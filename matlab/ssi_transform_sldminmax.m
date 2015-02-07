function [y, hist] = ssi_transform_sldminmax (x, sr, dlen, params, hist)
%
% sliding min/max 
%
% [mvgmin, mvgmax, hist] = ssi_transform_sldminmax (x, sr, dlen, params, hist = [])
%
% input:
%   x                       x
%   sr                   	sample rate
%   params
%                           k = number of bins used to approximate min/max
%                           w = window size in seconds
%   hist                    history
%
% output:
%   y                       minimum series followed by maximum series
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
%

if nargin < 4
    help ssi_transform_sldminmax
    error ('not enough arguments')
end

% params
w = params.w;

[len, dim] = size (x);
N = round (w * sr);
alpha = repmat (1 - (2*sqrt(3)) ./ N, 1, dim);

if nargin < 5 | isempty (hist)
	hist.min = x(1,:);
    hist.max = x(1,:);
end

sldmin = zeros (len, dim);
sldmax = zeros (len, dim);
minval = hist.min;
maxval = hist.max;
for i = 1:len   
    sample = x(i,:);    
    minval = min (sample, alpha .* minval + (1 - alpha) .* sample);
    maxval = max (sample, alpha .* maxval + (1 - alpha) .* sample);    
    sldmin(i,:) = minval;    
    sldmax(i,:) = maxval;
end
hist.min = minval;
hist.max = maxval;

y = [sldmin sldmax];

