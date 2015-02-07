function [y, hist] = ssi_transform_powmean (x, sr, dlen, params, hist)
%
% Calculates power mean of a signal
%
% [y, hist] = ssi_transform_powmean (x, sr, dlen, params, hist)
%
% input:
%   x                       input signal
%   sr                      sample rate
%   dlen                    #delta samples
%   params
%                           p = filter coefficients
%                           p -> -Inf      miminum
%                           p =  -1        harmonic mean
%                           p -> 0         geometric mean
%                           p =  1         arithmetic mean
%                           p =  2         quadratic mean (RMS)
%                           p -> Inf       maximum
%   hist                    history
%
% output:
%   y                       output signal
%   hist                    history for recursive call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2010

if nargin < 4
    help ssi_transform_powmean
    error ('not enough arguments')
end

p = params.p;

[len dim] = size (x);

if dim == 1
    y = (sum (x.^p) ./len).^(1/p);
else
    y = (sum (x.^p,1) ./len).^(1/p);
end

hist = [];