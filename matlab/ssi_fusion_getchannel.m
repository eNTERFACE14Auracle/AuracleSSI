function channel = ssi_fusion_getchannel (samples, dims, nchannel)
% Returns samples of n'th channels.
%
%  channel = ssi_fusion_getchannel (samples, dims, nchannel)
%   
%   input:
%   samples:  sample matrix
%   dims:     vector with dimensions
%   nchannel: n'th channel
%
%   output:
%   channel:  sample matrix of n'th channel
%
%   2010, Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>

from = 1;
if nchannel > 1
    from = sum (dims(1:nchannel-1)) + 1;
end
to = from + dims(nchannel) - 1;

channel = samples(:,from:to);