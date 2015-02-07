function [y, hist] = ssi_transform_remavg (x, sr, dlen, params, hist)
%
% removes moving/sliding average from signal
%
% [y, hist] = ssi_transform_remavg (x, sr, dlen, params, hist = [])
%
% input:
%   x                       input signal
%   sr                   	sample rate
%   params
%                           func = pointer to /sliding average function (mvgavg)
%                           ws = window size in s (5)
%   hist                    history
%
% output:
%   y                       output signal
%   hist                    history for recursive call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2010
%

if nargin < 3
    help ssi_transform_remavg
    error ('not enough arguments')
end

if nargin < 4 || isempty (params)
   params.func = @ssi_transform_mvgavg;
   params.ws = 5;
end

if nargin < 5 || isempty (hist)
    hist = [];
end

[avg, hist] = params.func (x, sr, dlen, params, hist);
y = x - avg;



