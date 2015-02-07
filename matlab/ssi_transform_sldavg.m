function [y, hist] = ssi_transform_sldavg (x, sr, dlen, params, hist)
%
% sliding average
%
% [y, hist] = ssi_transform_sldavg (x, sr, dlen, params, hist = [])
%
% input:
%   x                       input signal
%   sr                   	sample rate
%   dlen                    #delta samples
%   params                  ws = window size in seconds (5)
%                           (if ws is vector sliding average
%                           is calculated for each window size)
%   hist                    history (default: [])
%
% output:
%   y                       output signal
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
%

if nargin < 3
    help ssi_transform_sldavg
    error ('not enough arguments')
end

if nargin < 4 || isempty (params)
    params.ws = 5;
end

% read params
ws = params.ws;

[len, dim] = size (x);
Ns = round (ws * sr);
number = length (Ns);
alpha = repmat (1 - (2*sqrt(3)) ./ Ns(:)', 1, dim);

inds = repmat (1:dim, number, 1);
x = x(:,inds(:));

if nargin < 5 || isempty (hist)
	hist.avg = x(1,:);
end

y = zeros (len, dim*number);
avg_ = hist.avg;
for i = 1:len
    
    sample = x(i,:);
    avg_ = alpha .* avg_ + (1 - alpha) .* sample;        
    y(i,:) = avg_;
end
hist.avg = avg_;







