function [y, hist] = ssi_transform_minmaxnorm (x, sr, dlen, params, hist)
% moving min/max 
%
% [mvgmin, mvgmax, hist] = ssi_transform_minmaxnorm (x, sr, dlen, params, hist = [])
%
% input:
%   x                       signal
%   sr                   	sample rate
%   params
%                           mmf = minmax function (mvgminmax)
%                           mmp = minmax function params (k=10,w=10)
%                                (see ssi_transform_[mvg,sld]minmax)
%                           range = normalization interval in seconds ([0 1])
%   hist                    history
%
% output:
%   y                       normalized signal
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
%

if nargin < 3
    help ssi_transform_mvgminmax
    error ('not enough arguments')
end

if nargin < 4 || isempty (params)
   params.mmf = @ssi_transform_mvgminmax;
   params.mmp.k = 10;
   params.mmp.w = 10;
   params.range = [0 1];
end

[len, dim] = size (x);

% params
mmf = params.mmf;
mmp = params.mmp;
range = params.range;

if nargin < 5 | isempty (hist)
    hist = [];
end

[mm, hist] = mmf (x, sr, dlen, mmp, hist);
y = ssi_normalize (x, range, false, mm(:,1:dim), mm(:,dim+1:end));



