function [samples, classes, class_names, users, user_names] = ssi_samples_read_old (filename, stream_index)
%
% read sample list from file
%
% [samples, classes, class_names, users, user_names] = ssi_samples_read (filename, stream_index)
%
% input:
%   filename                filename
%   stream_index            stream index (1,2,...)
%
% output:
%   samples                 matrix with samples (#sample x #feature)
%   classes                 vector with class labels
%   class_names             cell string with class names
%   users                   vector with user labels
%   user_names              cell string with user names
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


fid = fopen (filename);
if fid == -1
    error ('cannot open file');
end

n_user = fread (fid, 1, 'uint32');
user_names = cell (n_user, 1);
for n = 1:n_user
    user_names{n} = readLine (fid);
end

n_class = fread (fid, 1, 'uint32');
class_names = cell (n_class, 1);
for n = 1:n_class
    class_names{n} = readLine (fid);
end

while 1
    [class, user, time, prob, num, sample] = read_next_sample (fid, stream_index);
    if isempty (sample)
        break;
    end
    samples = [samples; sample];
    classes = [classes; class];
    users = [users; user];
end
 
fclose (fid);
 
function [class, user, time, prob, num, sample] = read_next_sample (fid, stream_index)

sample = [];

user = fread (fid, 1, 'uint32') + 1;
class = fread (fid, 1, 'uint32') + 1;
time = fread (fid, 1, 'float64');
prob = fread (fid, 1, 'float32');
num = fread (fid, 1, 'uint32');
for i = 1:num
    if i == stream_index
        [sample, sr, inds, time] = ssi_signal_read (fid, true);
    else
        ssi_signal_read (fid, true);
    end
end
 
 
function string = readLine (fid)
     
len = fread (fid, 1, 'uint32');
string = fread (fid, len, '*char')';
    