function [samples, sr, classes, cnames, users, unames, times] = ssi_samples_read (path, stream_index, as_matrix)
%
% read sample list from file
%
% [samples, sr, classes, cnames, users, unames, times] = ssi_samples_read (path, stream_index, as_matrix)
%
% input:
%   path                    path
%   stream_index            stream index (1,2,...)
%   as_matrix               return samples as matrix (note: ignores everything but first frame)
%
% output:
%   samples                 matrix with samples (#sample x #feature)
%   sr                      sample rate
%   classes                 vector with class labels
%   cnames                  cell string with class names
%   users                   vector with user labels
%   unames                  cell string with user names
%   timestamps              matrix with start time in ms
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/08/23

samples = [];
classes = [];
users = [];

if nargin < 1
    help ssi_samples_read   
    error ('not enough arguments')
end

if nargin < 2
    stream_index = 1;
end

if nargin < 3
    as_matrix = 0;
end

[pathstr, name, ext] = fileparts (path);
if isempty (ext)
    path_info = [path '.samples'];
else
    path_info = path;
end
path_data = [path_info '~'];

xDoc = xmlread (path_info);
xSamples = xDoc.getDocumentElement;
version = str2double (xSamples.getAttribute('ssi-v').toCharArray);

xNodes = xSamples.getChildNodes;
xInfo = xNodes.item(1);
is_ascii = xInfo.getAttribute ('ftype').compareToIgnoreCase ('ASCII') == 0;
n_samples = str2double (xInfo.getAttribute ('size').toCharArray');
n_missing = str2double (xInfo.getAttribute ('missing').toCharArray');

xNodes = xInfo.getChildNodes;
xStreams = xNodes.item(1);
xNodes = xStreams.getChildNodes;
xStream = xNodes.item(1 + (stream_index-1) * 2);
sr = str2double (xStream.getAttribute ('sr').toCharArray');
dim = str2double (xStream.getAttribute ('dim').toCharArray');
byte = str2double (xStream.getAttribute ('byte').toCharArray');
type_s = xStream.getAttribute ('type').toCharArray';

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
    otherwise
        error (['type ' type_s ' not supported']);
end

xNodes = xInfo.getChildNodes;
xClasses = xNodes.item(3);
xNodes = xClasses.getChildNodes;
n_classes = (xNodes.getLength - 1) / 2;
cnames = cell (n_classes, 1);
csizes = zeros (n_classes, 1);
for i = 1:n_classes
    cnames{i} = xNodes.item(2*i-1).getAttribute ('name').toCharArray';
    csizes(i) = str2double (xNodes.item(2*i-1).getAttribute ('size').toCharArray');
end

xNodes = xInfo.getChildNodes;
xUsers = xNodes.item(5);
xNodes = xUsers.getChildNodes;
n_users = (xNodes.getLength - 1) / 2;
unames = cell (n_users, 1);
usizes = zeros (n_users, 1);
for i = 1:n_users
    unames{i} = xNodes.item(2*i-1).getAttribute ('name').toCharArray';
    usizes(i) = str2double (xNodes.item(2*i-1).getAttribute ('size').toCharArray');
end

if is_ascii
    warning ('not implemented');
else
    fid = fopen (path_data);       
    if as_matrix == 1
        samples = nan (n_samples, dim);
    else
        samples = cell (n_samples, 1);
    end
    times = zeros (n_samples, 1);
    classes = zeros (n_samples, 1);
    users = zeros (n_samples, 1);
    for i = 1:n_samples
        [class, user, time, prob, num, sample] =  read_next_sample_b (fid, dim, type, stream_index);
        if as_matrix == 1
            if ~isempty (sample)             
                samples(i,:) = sample(1,:);
            end
        else
            samples{i} = sample;
        end
        times(i) = time;
        classes(i) = class;
        users(i) = user;
    end    
    fclose (fid);
end


function [class, user, time, prob, num, sample] = read_next_sample_b (fid, dim, type, stream_index)

sample = [];

user = fread (fid, 1, 'uint32') + 1;
class = fread (fid, 1, 'uint32') + 1;
time = fread (fid, 1, 'float64');
prob = fread (fid, 1, 'float32');
num = fread (fid, 1, 'uint32');
for i = 1:num
    n = fread (fid, 1, 'uint32');
    data = fread (fid, n * dim, type);    
    if i == stream_index
        sample = reshape (data, dim, n)';
    end
end


% 
% 
% 
% fid = fopen (filename);
% if fid == -1
%     error ('cannot open file');
% end
% 
% n_user = fread (fid, 1, 'uint32');
% user_names = cell (n_user, 1);
% for n = 1:n_user
%     user_names{n} = readLine (fid);
% end
% 
% n_class = fread (fid, 1, 'uint32');
% class_names = cell (n_class, 1);
% for n = 1:n_class
%     class_names{n} = readLine (fid);
% end
% 
% while 1
%     [class, user, time, prob, num, sample] = read_next_sample (fid, stream_index);
%     if isempty (sample)
%         break;
%     end
%     samples = [samples; sample];
%     classes = [classes; class];
%     users = [users; user];
% end
%  
% fclose (fid);
%  
%  
%  
% function string = readLine (fid)
%      
% len = fread (fid, 1, 'uint32');
% string = fread (fid, len, '*char')';
    