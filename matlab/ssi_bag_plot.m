function ssi_bag_plot (bag)
%
% plot bag
%
% subbag = ssi_bag_plot (bag)
%
% input:
%   bag          the bag
%                                                          
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 1
    help bagplot
    error ('not enough arguments')
end

for nbag = 1:length (bag)
    figure;
    datafields = fieldnames (bag(nbag).data);
    ndatas = length (datafields);
    wavefields = fieldnames (bag(nbag).wave);
    nwaves = length (wavefields);
    annofields = fieldnames (bag(nbag).anno);
    nannos = length (annofields);
    nplots = ndatas + nwaves;
    nplot = 0;
    for ndata = 1:ndatas
        nplot = nplot + 1;
        subplot (nplots,1,nplot);
        entry = bag(nbag).data.(datafields{ndata});
        title (entry.path, 'Interpreter', 'none');        
        ssi_signal_plot (entry.signal, entry.sr);               
%         plotanno (bag(nbag).anno);                
    end
    for nwave = 1:nwaves
        nplot = nplot + 1;
        subplot (nplots,1,nplot);
        entry = bag(nbag).wave.(datafields{nwave});
        title (strrep (entry.path, '\', '\\'));
        ssi_signal_plot (entry.signal, entry.sr);       
    end
end
