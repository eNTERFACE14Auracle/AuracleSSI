function [rate, hist] = ssi_transform_pulse (frame, sr, dlen, params, hist)
%
% pulse
%
% [y, hist] = ssi_transform_pulse (x, sr, dlen, params, hist = [])
%
% input:
%   x                       signal
%   sr                   	sample rate
%   dlen                    #delta samples
%   params                  
%                           delta = delta for max search
%                           min_rate = minimum rate
%                           max_rate = maximum rate
%   hist                    history
%
% output:
%   y                       pulse signal
%   hist                    history for next call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2010

if nargin < 4
    help ssi_transform_pulse;
    error ('not enough argument')
end

delta = params.delta;
min_rate = params.min_rate;
max_rate = params.max_rate;

if nargin < 5 | isempty (hist)
    hist.offset = 0;
    hist.last = 0;
    hist.minval = Inf; 
    hist.maxval = -Inf;
    hist.minpos = NaN; 
    hist.maxpos = NaN;
    hist.last = 0;
    hist.last_dist = 0;    
    hist.lookformax = 0;
    hist.rate = (min_rate + max_rate) / 2;
    hist.peaks = [];
end

[num dim] = size (frame);

% read history
offset = hist.offset;
last = hist.last;
last_dist = hist.last_dist;
maxval = hist.maxval;
minval = hist.minval;
minpos = hist.minpos;
maxpos = hist.maxpos;
lookformax = hist.lookformax;
rate = hist.rate;
peaks = hist.peaks;

% contraints
min_dist = round ((60 ./ max_rate) * sr); 
max_dist = round ((60 ./ min_rate) * sr);

% find maxima
dists = [];
for i=1:num
    
    this = frame(i);
    if this > maxval, maxval = this; maxpos = offset+i; end
    if this < minval, minval = this; minpos = offset+i; end

    if lookformax
        if this < maxval - delta
            minval = this; 
            minpos = offset+i;
            % decide if we keep it
            candidate = maxpos - last;
            if candidate >= min_dist %& candidate > 0.6 * last_dist
                % decide if add extra
                if candidate <= max_dist %& (last_dist == 0 | candidate < 1.4 * last_dist)
                    % add candidate
                    dists = [dists; candidate];
                    last_dist = candidate;
                end
                last = maxpos;                
            end
            lookformax = 0;
        end
    else
        if this > minval + delta
            maxval = this; 
            maxpos = offset+i;            
            lookformax = 1;
        end
    end
end

if not (isempty (dists))
    rate = (rate + 60 / (mean (dists ./ sr))) / 2; 
    if isempty (peaks)
        peaks = cumsum (dists);
    else
        peaks = [peaks; peaks(end) + cumsum(dists)];
    end
end

% adjust history
hist.offset = hist.offset + num;
hist.last = last;
hist.last_dist = last_dist;
hist.minval = minval;
hist.maxval = maxval;
hist.minpos = minpos;
hist.maxpos = maxpos;
hist.lookformax = lookformax;
hist.rate = rate;
hist.peaks = peaks;
