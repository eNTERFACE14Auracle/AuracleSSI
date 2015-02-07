function [y mins maxs] = ssi_normalize (x, range, glob, mins, maxs)
%
% normalize using fixed interval
%
% y = ssi_normalize (x, range = [0 1], glob = false, mins = min(x), maxs = max(x))
%
% input:
%   x                   signal
%   range               interval
%   glob                global normalization (not dimensionwise)
%   mins                provide extern minima
%   maxs                provide extern maxima
%
% output:
%   y                   normalized signal
%   mins                minima used during normalization
%   maxs                maxima used during normalization
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2010

[len dim] = size (x);

if nargin < 2 | isempty (range)
    range = [0 1];
end
if nargin < 3 | isempty (glob)
    glob = false;
end
if nargin < 4 | isempty (mins)       
    mins = repmat (min (x, [], 1), len, 1);
end
if nargin < 5 | isempty (maxs)
    maxs = repmat (max (x, [], 1), len, 1);
end

if glob
    mins = repmat (min (mins,[],2), 1, dim);
    maxs = repmat (max (maxs,[],2), 1, dim);
end

s = warning('off','MATLAB:divideByZero');
y = (x - mins) ./ (maxs - mins);
s = warning('on','MATLAB:divideByZero');
y(isnan (y)) = 0;
y = range(1) + y * (range(2) - range(1));

mins = mins(1,:);
maxs = maxs(1,:);
