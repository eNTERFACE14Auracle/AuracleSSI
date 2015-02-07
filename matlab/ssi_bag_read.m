function bag = ssi_bag_read (bag, varargin)
%
% read bag (data, wav, anno)
%
% bag = ssi_bag_read (bag, varargin)
%
% input:
%   bag          if not empty files will be added to existing bag
%   varargin     a list of filenames (may include wildcards)
%
% output:
%   bag          struct of following format:
%
%                bag
%                 |-> anno
%                     |-> path
%                     |-> from
%                     |-> to
%                     |-> labels
%                     |-> lnames
%                 |-> data
%                     |-> path
%                     |-> signal
%                     |-> sr
%                     |-> inds
%                     |-> time
%                 |-> wave
%                     |-> path
%                     |-> signal
%                     |-> sr
%                                                          
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 2
    help bagload
    error ('not enough arguments')
end

entry = struct ('anno', struct (), 'data', struct (), 'wave', struct ());

for nvar = 1:length (varargin)
            
    pathstr = fileparts(varargin{nvar});
    files = dir (varargin{nvar});
    
    for nfile = 1:length (files)
                        
        if files(nfile).isdir
            continue
        end
        
        file = fullfile (pathstr, files(nfile).name);
        [xxx, name, ext] = fileparts(file);
        
        switch ext
            
            case '.anno'
                                
                anno = ssi_anno_read (file);
                nannos = size (anno, 1);
                lnames = unique (anno(:,3));
                
                labels = zeros (nannos,1);
                for nlname = 1:length (lnames)
                    labels = labels + nlname * strcmp (lnames{nlname}, anno(:,3));
                end
                
                tmp = struct ();                                
                tmp.path = file;
                tmp.from = [anno{:,1}]';
                tmp.to = [anno{:,2}]'; 
                tmp.labels = labels; 
                tmp.lnames = lnames;
                                
                entry.anno.(name) = tmp;                
                
            case '.data'
                
                [signal, sr, inds, time] = ssi_signal_read (file);
                tmp = struct ('path', file, 'signal', signal, 'sr', sr, 'inds', inds, 'time', time);                                
                
                entry.data.(name) = tmp;
                
            case '.wav'
                
                [signal, sr] = ssi_wave_read (file);
                tmp = struct ('path', file, 'signal', signal, 'sr', sr);                                

                entry.wave.(name) = tmp;

                
            otherwise
                s = strrep (file, '\', '\\');
                warning ('SSI:ssi2mat', ['unsupported file type in ''' s '''']);
        end
        
    end
end

if isempty (bag)
    bag = entry;
else
    bag(end+1) = entry;
end





