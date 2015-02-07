function ssi_selection_write (filepath, indices, scores)
%
% stores feature selection in ascii format
%
% input:
%   filepath                filepath
%   indices                 vector with indices
%   scores                  vector with scores
%
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2011/03/28
% created: 2011/03/28
% 

if nargin < 3
    help ssi_selection_write
    error ('not enough arguments')
end

n = length (indices);

fid = fopen (filepath, 'wt');
fprintf (fid, '%u\n%u\n', n, n);
for i=1:n
    fprintf (fid, '%u %f\n', indices(i), scores(i));
end
fclose (fid);
