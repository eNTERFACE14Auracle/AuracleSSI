function [y, hist] = ssi_transform_mvgminmax (x, sr, dlen, params, hist)
% moving min/max 
%
% [mvgmin, mvgmax, hist] = ssi_transform_mvgminmax (x, sr, dlen, params, hist = [])
%
% input:
%   x                       signal
%   sr                   	sample rate
%   params
%                           k = number of bins used to approximate min/max (10)
%                           w = window size in seconds (10)
%   hist                    history
%
% output:
%   y                       minimum series followed by maximum series
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
%

if nargin < 3
    help ssi_transform_mvgminmax
    error ('not enough arguments')
end

if nargin < 4 || isempty (params)
   params.k = 10;
   params.w = 10;
end


% params
w = params.w;
k = params.k;

% x size
[len, dim] = size (x);
N = round (w * sr);
wk = round (N / k);

% if necessary init history now
if nargin < 5 | isempty (hist)
    hist.mins = nan (k, dim);
    hist.maxs = nan (k, dim);
    hist.count = 0;
    hist.head = 1;
end

% allocate result matrix
mvgmin = zeros (len, dim);
mvgmax = zeros (len, dim);

% retrieve history from last call
count = hist.count;
mins = hist.mins;
maxs = hist.maxs;
head = hist.head;

for i = 1:len
    sample = x(i,:);
    if mod (count, wk) == 0
        if head == k
            head = 1;
        else
            head = head + 1;
        end
        mins(head,:) = sample;
        maxs(head,:) = sample;
    else        
        mins(head,:) = min (mins(head,:), sample);
        maxs(head,:) = max (maxs(head,:), sample);
    end
    count = count + 1;
    mvgmin(i,:) = min (mins);
	mvgmax(i,:) = max (maxs);
end

% store history for next call
hist.count = count;
hist.mins = mins;
hist.maxs = maxs;
hist.head = head;

y = [mvgmin mvgmax];

