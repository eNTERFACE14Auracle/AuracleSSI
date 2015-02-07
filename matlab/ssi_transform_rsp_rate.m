function [y, hist] = ssi_transform_rsp_rate (x, sr, dlen, params, hist)
%
% Extracts heart rate from a rsp signal
%
% [y, hist] = ssi_transform_bvp_rate (x, sr, dlen, params, hist)
%
% input:
%   x                       input signal
%   sr                      sample rate
%   dlen                    #delta samples
%   params                  
%                           func = average function used to remove trend (mvgavg)
%                           ws = window size in s used by averate function (5)
%                           cutoff = lowpass cut off frequency in Hz (0.2)
%                           order = lowpass filter order (3)
%                           delta = smallest difference between two successive maxima (0.1)
%                           min_rate = minimum rate in bpm (5)
%                           max_rate = maximum rate in bpm (50)
%   hist                    history
%
% output:
%   y                       output signal
%   hist                    history for recursive call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2010

if nargin < 3
    help ssi_transform_rsp_rate
    error ('not enough arguments')
end

if nargin < 4 | isempty (params) 
    params.func = @ssi_transform_mvgavg;
    params.ws = 5;      
    params.cutoff = 0.2;
    params.order = 3;
    params.delta = 0.1;
    params.min_rate = 5;
    params.max_rate = 50;      
end

if nargin < 5 | isempty (hist)
    hist.remavg = [];
	hist.butfilt = [];    
    hist.pulse = [];
end

[x_rem, hist_remavg] = ssi_transform_remavg (x, sr, dlen, params, hist.remavg);
hist.remavg = hist_remavg;

params.type = 'low';
[x_low, hist_butfilt] = ssi_transform_butfilt (x_rem, sr, dlen, params, hist.butfilt);
hist.butfilt = hist_butfilt;

[y, hist_pulse] = ssi_transform_pulse (x_low, sr, dlen, params, hist.pulse);
hist.pulse = hist_pulse;
