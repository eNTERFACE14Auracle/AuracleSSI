function [y, hist] = ssi_transform_template (x, sr, dlen, params, hist)
%
% Template
%
% [y, hist] = ssi_transform_template (x, sr, dlen, params, hist)
%
% input:
%   x                       input signal
%   sr                      sample rate
%   dlen                    #delta samples
%   params
%   hist                    history
%
% output:
%   y                       output signal
%   hist                    history for recursive call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2010

if nargin < 4
    help ssi_transform_template
    error ('not enough arguments')
end

y = x;
hist = [];