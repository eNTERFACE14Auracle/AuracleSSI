function segs = ssi_signal_cut (data, anno, offset)

if nargin < 3 | isempty (offset)
    offset = 0;
end

signal = data.signal;
sr = data.sr;
from = anno.from;
to = anno.to;

n_segs = size (from, 1);

segs = cell (n_segs, 1);
for i = 1:n_segs
    start = 1 + round (sr * (from(i) - offset));
    stop = 1 + round (sr * (to(i) + offset));
    segs{i} = signal(start:stop,:);
end