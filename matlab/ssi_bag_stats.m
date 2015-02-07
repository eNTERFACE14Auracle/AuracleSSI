function stats = ssi_bag_stats (bag, name)
%
% calculates statitics over with that name
%
% bag = ssi_bag_stats (bag)
%
% input:
%   bag          the bag
%
% output:
%   stats        statistics
%                                                          
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 1
    help ssi_bag_stats;
    error ('too few arguments')
end

minlen = Inf;
maxlen = -Inf;
minval = [];
maxval = [];

for nbag = 1:length (bag)
    
    datafields = fieldnames (bag(nbag).data);
    ndatas = length (datafields);       
    
    for ndata = 1:ndatas
        if strcmp (datafields{ndata}, name)
            entry = bag(nbag).data.(datafields{ndata});
            x = entry.signal;
            x_sr = entry.sr;       
            minlen = min (minlen, length (x) / x_sr);
            maxlen = max (maxlen, length (x) / x_sr);
            if isempty (minval)
                minval = min (x);
                maxval = max (x);
            else
                minval = min (minval, min (x));
                maxval = max (maxval, max (x));
            end
            
        end
    end
    
end

stats.minlen = minlen;
stats.maxlen = maxlen;
stats.minval = minval;
stats.maxval = maxval;