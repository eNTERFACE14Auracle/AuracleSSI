function [y, y_sr, hist] = ssi_transform (x, sr, func, params, fs, ds)
%
% read signal from file
%
% [x, sr, hist] = ssi_transform (x, sr, func, params, fs = 0, ds = 0)
%
% input:
%   x                       input signal
%   sr                      sample rate of input signal
%   func                    function pointer
%   params                  function parameters
%   fs                      frame size in samples
%   ds                      delta size in samples
%
% output:
%   y                       output signal
%   y_sr                    sample rate of output signal
%   hist                    history
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 3
    help ssi_transform;
    error ('too few arguments')
end

if nargin < 4
    params = [];
end

if nargin < 5 || isempty (fs)
    fs = 0;
end

if nargin < 6 || isempty (ds)
    ds = 0;
end

[len, dim] = size (x);
hist = [];

if (fs <= 0) 
    
    [y, hist] = func (x, sr, 0, params);
    [len_y, dim_y] = size (y);
    y_sr = sr * (len_y / len);
    
else

    [ws, num] = ssi_wins (x, sr, fs, ds);

    frame = x(ws(1,:),:);
    [y, hist] = func (frame, sr, ds, params);

    [len_y, dim_y] = size (y);
    y_ws = reshape (1:num*len_y, len_y, num); 
    y_sr = sr * (len_y / fs);
    y = repmat (y, num, 1); 

    for i = 2:num
        frame = x(ws(i,:),:);
        [out, hist] = func (frame, sr, ds, params, hist);
        y(y_ws(:,i),:) = out;
    end

end