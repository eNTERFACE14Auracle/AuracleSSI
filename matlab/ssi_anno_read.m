function anno = ssi_anno_read (filename)
%
% read annotation file
%
% anno = ssi_anno_read (filename)
%
% input:
%   filename                filename
%
% output:
%   anno                    annotation (start, end, label)
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/15

fid = fopen (filename);
           
anno = {};
counter = 1;
while 1
    rem = fgetl(fid);
    if ~ischar(rem)
        break
    end
    if isempty (rem)
        continue;
    end
    [tok, rem] = strtok (rem);
    anno{counter,1} = str2double (tok);
    [tok, rem] = strtok (rem);
    anno{counter,2} = str2double (tok);
    [tok, rem] = strtok (rem);
    anno{counter,3} = tok;
    counter = counter + 1;
end

fclose (fid);