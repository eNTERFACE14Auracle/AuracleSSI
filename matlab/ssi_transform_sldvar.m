function [y, hist] = ssi_transform_sldvar (x, sr, dlen, params, hist)
%
% sliding variance
%
% [y, vrc, hist] = ssi_transform_sldvar (x, sr, dlen, params, hist = [])
%
% input:
%   x                       signal
%   sr                   	sample rate
%   dlen                    #delta samples
%   params                  ws = window size in seconds
%                           (if ws is vector sliding variance
%                           is calculated for each window size)
%   hist                    history (default: [])
%
% output:
%   y                       variance series
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
%

if nargin < 4
    help ssi_transform_sldvar
    error ('not enough arguments')
end

% read params
ws = params.ws;

[len, dim] = size (x);
Ns = round (ws * sr);
number = length (Ns);
alpha = repmat (1 - (2*sqrt(3)) ./ Ns(:)', 1, dim);

inds = repmat (1:dim, number, 1);
x = x(:,inds(:));

if nargin < 5 | isempty (hist)
	hist.avg = x(1,:);
    hist.vrc = zeros (1,number*dim);
end

y = zeros (len, dim*number);
avg_ = hist.avg;
vrc_ = hist.vrc;
for i = 1:len
    
    sample = x(i,:);
    avg_ = alpha .* avg_ + (1 - alpha) .* sample;    
    vrc_ = alpha .* vrc_ + (1 - alpha) .* (sample - avg_).^2;
    y(i,:) = vrc_;
 
end
hist.avg = avg_;
hist.vrc = vrc_;







