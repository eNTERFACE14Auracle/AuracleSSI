function class = ssi_model_getclass (samples, labels, nclass)
% Returns samples of n'th class.
%
%  channel = ssi_model_getclass (samples, nclass)
%   
%   input:
%   samples:  sample matrix
%   labels: label vector
%   nclass: n'th class
%
%   output:
%   class:  sample matrix of n'th class
%
%   2010, Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>

class = samples (labels == nclass, :);