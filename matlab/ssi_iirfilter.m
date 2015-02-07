function [y, hist] = ssi_iirfilter (x, sos, hist)
%
% Infinite response filter (IIR)
%
% [y, hist] = ssi_iirfilter (x, sos, hist)
%
% input:
%   x                       input signal
%   coefs                   filter coefficients
%   hist                    history
%
% output:
%   y                       output signal
%   hist                    history for recursive call
%
% also see butcoefs.m
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
% 

if nargin < 2
    help ssi_iirfilter
    error ('not enought arguments');
end

sections = size (sos, 1);
y = zeros (size (x));
if nargin < 3 || isempty (hist)
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
