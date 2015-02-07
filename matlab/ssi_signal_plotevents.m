function ssi_signal_plotevents (timings, labels, color, varargin)
%
% Function to plot events to a signal
%
% ssi_signal_plotevents (timings, [labels], [color], varargin)
%
% input:
%   times                   vector with start and stop times of each event (in seconds)                
%   labels                  vector with label names or a single name
%   color                   color
%   varargin                additional plot arguments
%
%
% 2012, Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% 

if nargin < 1
    help ssi_signal_plotevents;
    error ('wrong number of argument');
end

if nargin < 2 || isempty (labels)
    labels = [];
end

if nargin < 3 || isempty (color)
    color = 'b';
end

handle = gca;
hold on;
  
x_lims = get (handle, 'XLim');
y_lims = get (handle, 'YLim');
axis ([x_lims(1) x_lims(2) y_lims(1) y_lims(2)]);

% highlight events
n_timings = size (timings,1);
for j = 1:n_timings
	et = timings(j,:);    
    fill ([et(1) et(2) et(2) et(1) et(1)], [y_lims(1) y_lims(1) y_lims(2) y_lims(2) y_lims(1)], color, 'LineStyle', 'none');
    if ~isempty (labels)
        if size (labels,1) > 1
            text (et(1) + (et(2) - et(1)) / 2, y_lims(1), labels{j}, 'HorizontalAlignment', 'center', 'VerticalAlignment', 'bottom');
        else
            text (et(1) + (et(2) - et(1)) / 2, y_lims(1), labels, 'HorizontalAlignment', 'center', 'VerticalAlignment', 'bottom');
        end
    end
end