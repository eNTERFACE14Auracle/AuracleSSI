function [y, hist] = ssi_transform_scbaseline (x, sr, dlen, params, hist)
%
% removes baseline from sc signal
%
% [mvgmin, mvgmax, hist] = ssi_transform_scbaseline (x, sr, dlen, params, hist = [])
%
% input:
%   x                       signal
%   sr                   	sample rate
%   params
%                           w = window size in s
%   hist                    history
%
% output:
%   y                       baseline signal
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
%

if nargin < 4
    help ssi_transform_mvgminmax
    error ('not enough arguments')
end

[len, dim] = size (x);

% params
if nargin < 5 | isempty (hist)
    hist = [];
end

[mm, hist] = ssi_transform_sldminmax (x, sr, dlen, params, hist);
y = x - mm(:,1:dim);



