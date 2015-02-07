function [x, sr, inds, time] = ssi_signal_read_old (filename, first_only, is_ascii)
%
% read signal from file
%
% [x, sr, inds, time] = ssi_signal_read_old (filename, first_only, is_ascii)
%
% input:
%   filename                filename or fid
%   first_only              if true only first sample is read
%   is_ascii                set true if input file is in ascii format
%
% output:
%   x                       signal
%   sr                      sample rate
%   inds                    start index of each signal block
%   time                    timestamp of each signal block
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

sr = 0;
inds = [1];
time = [];
x = [];

if nargin < 1
    help ssi_signal_read   
    error ('not enough arguments')
end

if nargin < 2
    first_only = false;
end

if nargin < 3
    is_ascii = false;
end

if is_ascii
    
   if ischar (filename)
        fid = fopen (filename, 'rt');
        if fid == -1
            error ('cannot open file');
        end
    else
        fid = filename;
    end
    
    fline = fgetl(fid); 
    vers = sscanf (fline, 'SSI@%d');

                      
    fline = fgetl(fid);
    A = sscanf (fline, '%g %u %u %u');
    sr = A(1); dim = A(2); bytes = A(3); typeid = A(4);
    
    switch typeid
        case 1
            type = 'int8';
            fstr = repmat ('%i ', 1, dim);
        case 2
            type = 'uint8';
            fstr = repmat ('%u ', 1, dim);
        case 3
            type = 'int16';
            fstr = repmat ('%i ', 1, dim);
        case 4
            type = 'uint16';
            fstr = repmat ('%u ', 1, dim);
        case 5
            type = 'int32';
            fstr = repmat ('%i ', 1, dim);
        case 6
            type = 'uint32';
            fstr = repmat ('%u ', 1, dim);
        case 7
            type = 'int64';
            fstr = repmat ('%i ', 1, dim);
        case 8
            type = 'uint64';
            fstr = repmat ('%u ', 1, dim);
        case 9
            type = 'single';
            fstr = repmat ('%f ', 1, dim);
        case 10
            type = 'double';
            fstr = repmat ('%f ', 1, dim);
        otherwise
            error ('type not supported')
    end    
    
    while 1
        
        fline = fgetl(fid);
        if fline == -1
            break;
        end
        
        A = sscanf (fline, '%g %u');
        time = [time; A(1)];
        num = A(2);
        inds = [inds; inds(end) + num];
        x_ = zeros (num, dim, type);
        for i = 1:num
            fline = fgetl (fid);
            A = sscanf (fline, fstr);
            x_(i,:) = A';
        end
        x = [x; x_];
        
    end

else

    if ischar (filename)
        fid = fopen (filename);
        if fid == -1
            error ('cannot open file');
        end
    else
        fid = filename;
    end

    id = fread (fid, 3, 'char');
    if isempty (id) % end of file
        return
    end
    if all (id == [83;83;73]) ~= 1
        error ('unkown format') 
    end

    vers = fread (fid, 1, 'char');

    switch vers

        case 1          

            sr = fread (fid, 1, 'double');
            dim = fread (fid, 1, 'uint32');
            bytes = fread (fid, 1, 'uint32');
            typeid = fread (fid, 1, 'char');    

            switch typeid
                case 1
                    type = 'int8';
                case 2
                    type = 'uint8';
                case 3
                    type = 'int16';
                case 4
                    type = 'uint16';
                case 5
                    type = 'int32';
                case 6
                    type = 'uint32';
                case 7
                    type = 'int64';
                case 8
                    type = 'uint64';
                case 9
                    type = 'single';
                case 10
                    type = 'double';
                otherwise
                    error ('type not supported')
            end    

            while 1
                timestamp = fread (fid, 1, 'double');
                if isempty (timestamp)
                    break;
                end
                time = [time; timestamp];
                len = fread (fid, 1, 'uint32');            
                inds = [inds; inds(end) + len];
                data = fread (fid, len * dim, type);
                if (dim > 1)
                    data = reshape (data, dim, len)';
                end  
                x = [x; data];
                if first_only
                    break;
                end
            end              

        otherwise        

            error ('unkown version') 

    end
end

if ischar (filename)
    fclose (fid);
end
