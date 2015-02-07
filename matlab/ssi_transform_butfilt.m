function [y, hist] = ssi_transform_butfilt (x, sr, dlen, params, hist)
%
% Butter filter (low, high or band)
%
% [y, hist] = ssi_transform_butfilt (x, sr, dlen, params, hist)
%
% input:
%   x                       signal
%   sr                      sample rate
%   dlen                    #delta samples
%   params                 
%                           order = filter order
%                           cutoff = cutoff frequency relative to nyquist
%                                    frequency, i.e. 1.0 = sr/2
%                                    in case of bandpass filter [low high]
%                           type = 'low', 'high', 'pass'
%                           zero = if not empty subtract first sample
%
% output:
%   y                       filtered signal
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
% 

if nargin < 4
    help ssi_transform_butfilt
    error ('not enough input arguments')    
end

[len dim] = size (x);

if nargin < 5 || isempty (hist)
    hist = cell (dim,1);
    if isfield (params, 'zero') && ~isempty (params.zero)
        params.zero = x(1,:);
    else
        params.zero = [];
    end
end

order = params.order;
cutoff = params.cutoff;
type = params.type;

y = zeros (len, dim);
coefs = ssi_butcoefs (order, sr, cutoff * sr/2, type);

if ~isempty (params.zero)
    x = x - repmat (params.zero, 1, dim);
end

for i = 1:dim
    [y(:,i), h] = ssi_iirfilter (x(:,i), coefs, hist{i});
    hist{i} = h;
end

if ~isempty (params.zero)
    y = y + repmat (params.zero, 1, dim);
end