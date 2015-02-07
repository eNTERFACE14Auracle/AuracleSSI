function [x, sr, inds, time] = ssi_signal_read (path)
%
% read signal from file
%
% [x, sr, inds, time] = ssi_signal_read (path)
%
% input:
%   path                    path
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
inds = [];
time = [];
x = [];

if nargin < 1
    help ssi_signal_read
    error ('not enough arguments')
end

[pathstr, name, ext] = fileparts (path);
if isempty (ext)
    path_info = [path '.stream'];
else
    path_info = path;
end
path_data = [path_info '~'];

xDoc = xmlread (path_info);
xStream = xDoc.getElementsByTagName ('stream');
version = str2double (xStream.item(0).getAttribute('ssi-v').toCharArray);

xInfo = xStream.item(0).getElementsByTagName ('info');
is_ascii = xInfo.item(0).getAttribute ('ftype').compareToIgnoreCase ('ASCII') == 0;
sr = str2double (xInfo.item(0).getAttribute ('sr').toCharArray');
dim = str2double (xInfo.item(0).getAttribute ('dim').toCharArray');
byte = str2double (xInfo.item(0).getAttribute ('byte').toCharArray');
type_s = xInfo.item(0).getAttribute ('type').toCharArray';
switch (type_s)
    case 'CHAR'
        type = 'int8';
        fstr = repmat ('%i ', 1, dim);
    case 'UCHAR'
        type = 'uint8';
        fstr = repmat ('%u ', 1, dim);
    case 'SHORT'
        type = 'int16';
        fstr = repmat ('%i ', 1, dim);
    case 'USHORT'
        type = 'uint16';
        fstr = repmat ('%u ', 1, dim);
    case 'INT'
        type = 'int32';
        fstr = repmat ('%i ', 1, dim);
    case 'UINT'
        type = 'uint32';
        fstr = repmat ('%u ', 1, dim);
    case 'LONG'
        type = 'int64';
        fstr = repmat ('%i ', 1, dim);
    case 'ULONG'
        type = 'uint64';
        fstr = repmat ('%u ', 1, dim);
    case 'FLOAT'
        type = 'single';
        fstr = repmat ('%f ', 1, dim);
    case 'DOUBLE'
        type = 'double';
        fstr = repmat ('%f ', 1, dim);
    case 'IMAGE'
        type = 'int8';
        meta = readMeta (xStream, type_s);
        %        fstr = repmat ('%u ', 1, meta.channels * meta.width * meta.height);
    otherwise
        error (['type ' type_s ' not supported']);
end

xChunks = xStream.item(0).getElementsByTagName ('chunk');
n_chunks = xChunks.getLength ();
time = zeros (n_chunks, 1);
inds = uint32 (zeros (n_chunks, 1));

nums = zeros (n_chunks, 1);
for i=0:n_chunks-1
    xChunk = xChunks.item(i);
    time(i+1) = str2double (xChunk.getAttribute ('from').toCharArray');
    nums(i+1) = str2double (xChunk.getAttribute ('num').toCharArray');
end
n_samples = sum (nums);
inds = [0; nums(1:end-1)];
time = [0; time(1:end-1)];

if strcmp (type_s, 'IMAGE') == 1
    if is_ascii
        ssi_err ('ascii image not supported');
    else
        fid = fopen (path_data);
        x = cell (n_samples, 1);
        for n=1:n_samples
            data = uint8 (128 + fread (fid, meta.channels * meta.width * meta.height, type));
            x{n} = reshape (data, meta.height, meta.width, meta.channels);
        end
    end
else
    if is_ascii
        fid = fopen (path_data);
        x = fscanf (fid, fstr, [dim, n_samples])';
        fclose (fid);
    else
        fid = fopen (path_data);
        data = fread (fid, n_samples * dim, type);
        x = reshape (data, dim, n_samples)';
        fclose (fid);
    end
end


function meta = readMeta (xStream, type)

if strcmp (type, 'IMAGE') == 1
    
    xMeta = xStream.item(0).getElementsByTagName ('meta');
    meta.width = str2double (xMeta.item(0).getAttribute ('width').toCharArray');
    meta.height = str2double (xMeta.item(0).getAttribute ('height').toCharArray');
    meta.depth = str2double (xMeta.item(0).getAttribute ('depth').toCharArray');
    meta.channels = str2double (xMeta.item(0).getAttribute ('channels').toCharArray');
    
else
    error (['no meta information parser for type ' type]);
end


