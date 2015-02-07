function [x, sr] = ssi_wave_read (filename, norm)
%
% read wave file
%
% [signal, sr] = ssi_wave_read (filename, norm = false)
%
% input:
%   filename                filename
%   norm                    normalize to interval [-1..1]
%
% output:
%   x                       signal
%   sr                      sample rate
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 1
    help ssi_wave_read   
    error ('not enough arguments')
end

if nargin < 2 | isempty (norm)
    norm = false;
end

[x, sr, nbits] = wavread (filename);

if norm
    x = x ./ (2^nbits / 2);
end