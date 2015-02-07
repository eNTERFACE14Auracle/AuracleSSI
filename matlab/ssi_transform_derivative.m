function [x_d, hist] = ssi_transform_derivative (x, sr, dlen, params, hist)
%
% Derivative
%
% [x_d, hist] = ssi_transform_derivative (x, sr, dlen, params, hist)
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

if nargin < 4
    help derivative
    error ('not enough arguments')
end

[len dim] = size (x);

delta = params.delta;
hist = [];

if nargin < 2
    
    x_d = zeros (len, dim);
    x_d(1:len-1,:) = diff (x);
    x_d(len,:) = x_d(len-1,:);
    
else

    [len, num] = size (x);
    x_d = zeros (size (x));

    data_ext_b = [repmat(x(1,:), delta, 1); x(1:2*delta,:)];
    len_ext_b  = size (data_ext_b, 1);
    data_ext_e = [x(len-2*delta+1:len,:); repmat(x(len,:), delta, 1)];
    len_ext_e  = size (data_ext_e, 1);
    for i = 1:delta
       x_d(1:delta,:) = x_d(1:delta,:) + i * (data_ext_b(delta+1+i:len_ext_b-delta+i,:) - data_ext_b(delta+1-i:len_ext_b-delta-i,:));
       x_d(delta+1:len-delta,:) = x_d(delta+1:len-delta,:) + i .* (x(delta+1+i:len-delta+i,:) - x(delta+1-i:len-delta-i,:));
       x_d(len-delta+1:len,:) = x_d(len-delta+1:len,:) + i .* (data_ext_e(delta+1+i:len_ext_e-delta+i,:) - data_ext_e(delta+1-i:len_ext_e-delta-i,:));   
    end
    x_d = x_d ./ (2 * sum ((1:delta).^2));

end