function [y, hist] = ssi_transform_ecg_rate (x, sr, dlen, params, hist)
%
% Extracts heart rate from a rsp signal
%
% [y, hist] = ssi_transform_ecg_rate (x, sr, dlen, params, hist)
%
% input:
%   x                       input signal
%   sr                      sample rate
%   dlen                    #delta samples
%   params                  
%                           cutoff = highpass cut off frequency in Hz (10)
%                           order = lowpass filter order (3)
%                           power = power (4)
%                           mmf = function to calculate moving min/max (mvgminmax)
%                           mmp.k = number of bins used to approximate min/max (5)
%                           mmp.w = window size in seconds (1)
%                           delta = smallest difference between two successive maxima (0.5)
%                           min_rate = minimum rate in bpm (10)
%                           max_rate = maximum rate in bpm (180)
%   hist                    history
%
% output:
%   y                       output signal
%   hist                    history for recursive call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2010

if nargin < 3
    help ssi_transform_ecg_rate
    error ('not enough arguments')
end

if nargin < 4 | isempty (params)   
    params.cutoff = 10;
    params.order = 3;
    params.power = 4;
    params.mmf = @ssi_transform_mvgminmax;
    params.mmp.k = 5;
    params.mmp.w = 1;
    params.delta = 0.5;
    params.min_rate = 10;
    params.max_rate = 180;      
end

if nargin < 5 | isempty (hist)
    hist.remavg = [];
	hist.butfilt = [];    
    hist.pulse = [];
    hist.norm = [];
end

params.type = 'high';
[x_high, hist_butfilt] = ssi_transform_butfilt (x, sr, dlen, params, hist.butfilt);
hist.butfilt = hist_butfilt;

x_pow = x_high .^ params.power;

params.range = [0 1];
[x_norm, hist_norm] = ssi_transform_minmaxnorm (x_pow, sr, dlen, params, hist.norm);
hist.norm = hist_norm;

[y, hist_pulse] = ssi_transform_pulse (x_norm, sr, dlen, params, hist.pulse);
hist.pulse = hist_pulse;
