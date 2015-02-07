function ssi_signal_plotmarker (signal, sr, indices, colors, varargin)
%
% Function to plot markers to a signal
%
% ssi_signal_plotmarker (signal, sr, indices, [colors], ...)
%
% input:
%   signal                  the signal
%   sr                      sample rate (or timestamps)
%   indices                 sample indices where markers should be placed
%   colors                  vector with color values
%   varargin                additional plot arguments
%
%
% 2012, Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% 

if nargin < 3
    help ssi_signal_plotmarker;
    error ('wrong number of argument');
end

if nargin < 4 || isempty (colors)
    colors = 'b';
end

n_times = size (signal, 1) / sr;
times = 0:1/sr:n_times - 1/sr;

% plot data
hold on;
plot (times(indices), signal(indices), 'o', 'MarkerEdgeColor', 'k', 'MarkerFaceColor', colors, varargin{:});
