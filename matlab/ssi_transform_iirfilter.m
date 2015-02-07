function [y, hist] = ssi_transform_iirfilter (x, sr, dlen, params, hist)
%
% Infinite response filter (IIR)
%
% [y, hist] = iirfilter (x, sr, dlen, params, hist)
%
% input:
%   x                       input signal
%   sr                      sample rate
%   dlen                    #delta samples
%   params
%                           coefs = filter coefficients
%   hist                    history
%
% output:
%   y                       output signal
%   hist                    history for recursive call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
% 

if nargin < 4
    help ssi_transform_iirfilter
    error ('not enought arguments');
end

sos = params.sos;

sections = size (sos, 1);
y = zeros (size (x));
if nargin < 5 | isempty (hist)
    hist = zeros (sections, 2);
end
for i = 1:length (x)
   y(i) = x(i);
   for j = 1:sections
        y(i) = y(i) - hist(j,1) * sos(j,5);
        new_hist = y(i) - hist(j,2) * sos(j,6);
        y(i) = new_hist * sos(j,1) + hist(j,1) * sos(j,2);
        y(i) = y(i) + hist(j,2) * sos(j,3);
        hist(j,2) = hist(j,1);
        hist(j,1) = new_hist;
   end
end
