function [ws, num] = ssi_wins (x, sr, fs, ds)
%
% Determines window intervals for a given frame and delta size
%
%   ws = ssi_wins (x, sr, fs, ds = 0)
%   
%   input:
%   x           signal
%   sr          sample rate
%   fs          frame size in samples
%   ds          delta size in samples
%
%   output:
%   ws          window intervals
%   num         #intervals
%
%   2007, Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>

if nargin < 3
    help ssi_wins;
    error ('too few arguments')   
end

if nargin < 4 | isempty (ds)
    ds = 0;
end

[len, dim] = size (x);

wlen    = fs + ds;
num     = fix ((len - ds) / fs);
ws = zeros (num, wlen);

ws = repmat (1:wlen, num, 1) + repmat (fs * (0:num-1)', 1, wlen);


