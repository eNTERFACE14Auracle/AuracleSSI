function [x, hist] = ssi_transform_hardlimit (x, sr, dlen, params, hist)
%
% hard limiting
%
% [y, hist] = ssi_transform_hardlimit (x, sr, dlen, params, hist = [])
%
% input:
%   x                       signal
%   sr                   	sample rate
%   dlen                    #delta samples
%   params                  
%                           thres = hard threshold
%   hist                    history
%
% output:
%   y                       hard limited signal
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008

if nargin < 4
    help ssi_transform_hardlimit
    error ('not enough arguments')
end

% read params
thres = params.thres;

[len dim] = size (x);

if nargin < 5 | isempty (hist)
    hist = [];
end

x(x > thres) = thres;









