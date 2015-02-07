function bag = ssi_bag_transform (bag, name, tname, func, params, fs, ds)
%
% transform bag
%
% bag = ssi_bag_plot (bag)
%
% input:
%   bag          the bag
%
% output:
%   bag          the bag including transformed signals
%                                                          
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 5
    help ssi_bag_transform;
    error ('too few arguments')
end

if nargin < 6 | isempty (fs)
    fs = 0;
end

if nargin < 7 | isempty (ds)
    ds = 0;
end

for nbag = 1:length (bag)
    
    datafields = fieldnames (bag(nbag).data);
    ndatas = length (datafields);       
    
    for ndata = 1:ndatas
        if strcmp (datafields{ndata}, name)
            entry = bag(nbag).data.(datafields{ndata});
            x = entry.signal;
            x_sr = entry.sr;
            [y, y_sr] = ssi_transform (x, x_sr, func, params, fs, ds);
            tentry.signal = y;
            tentry.sr = y_sr;
            [pathstr, xxx, ext] = fileparts (entry.path);
            tentry.path = fullfile (pathstr, [tname ext]);
            tentry.time = [];
            tentry.inds = [];
            bag(nbag).data.(tname) = tentry;
        end
    end
    
end
