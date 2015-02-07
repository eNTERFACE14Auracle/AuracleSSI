function ssi_signal_plot (signal, sr, label, style, varargin)
%
% Function to plot signal
%
% ssi_signal_plot (signal, sr, label, [style], ...)
%
% input:
%   signal                  the signal
%   sr                      sample rate (or timestamps)
%   label                   label of y axis
%   style                   line style (default: '-')  
%   varargin                additional plot arguments
%
%
% 2008, Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% 

if nargin < 2
    help ssi_signal_plot
    error ('not enough arguments')
end

if isempty (signal)
    return
end

if nargin < 3 || isempty (label)
    label = [];
end

if nargin < 4 || isempty (style)
    style = '-';
end

if isscalar (sr)
    time_len = size (signal,1) / sr;
    time = 0:1/sr:time_len - 1/sr;
else
    time = sr;
end

handle = gca;

if ishold
    y_lims = get (handle, 'YLim');
    x_lims = get (handle, 'XLim');    
    min_y = min (y_lims(1), min (signal(:)));
    max_y = max (y_lims(2), max (signal(:)));
    min_x = min (x_lims(1), time(1));
    max_x = max (x_lims(2), time(length(time)));    
else
    min_y = min (signal(:));
    max_y = max (signal(:));
    min_x = time(1);
    max_x = time(length(time));
end
if min_y == max_y || min_x == max_x || isnan (min_x) || isnan (min_y) || isnan (max_x) || isnan (max_y)
    return
end

% plot data
hold on
axis ([min_x max_x min_y max_y]);
plot (time, signal, style, varargin{:});
xlabel ('Seconds');
if ~isempty (label)
    ylabel (label);
end
