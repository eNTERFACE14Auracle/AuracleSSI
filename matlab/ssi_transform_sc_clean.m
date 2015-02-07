function [x, hist] = ssi_transform_sc_clean (x, sr, dlen, params, hist)
% moving average
%
%
% [x, hist] = ssi_transform_sc_clean (x, sr, dlen, params, hist)
%
% input:
%   x                       sc signal
%   sr                   	sample rate
%   dlen                    #delta samples
%   params                  
%                           w = average window
%                           thres = hard threshold
%   hist                    history
%
% output:
%   y                       cleaned signal
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008

if nargin < 4
    help ssi_transform_sc_clean
    error ('not enough arguments')
end

if nargin < 5 || isempty (hist)
    hist.baseline = [];
    hist.hardlimit = [];
end

[x, hist.baseline] = ssi_transform_scbaseline (x, sr, dlen, params, hist.baseline);
[x, hist.hardlimit] = ssi_transform_hardlimit (x, sr, dlen, params, hist.hardlimit);






