function subbag = ssi_bag_select (bag, inds, annos, datas, waves)
%
% select entries from bag
%
% subbag = ssi_bag_select (bag, inds = 'all', annos = 'all', datas = 'all', waves = 'all')
%
% input:
%   bag          the bag
%   indices      vector with indices (use 'all' to select all)
%   annos        cell with anno names (use 'all' to select all)
%   sessions     cell with data names (use 'all' to select all)
%   sessions     cell with wave names (use 'all' to select all)
%
% output:
%   subbag       bag with requested entries
%                                                          
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 1
    help bagsel
    error ('not enough arguments')
end

subbag = [];

if isempty (inds) 
    return;
end

if nargin < 2
    subbag = bag;
end

if strcmp (inds, 'all')
    inds = 1:length (bag);
end

if nargin < 3 | strcmp (annos, 'all')
    annos = allfields (bag, inds, 'anno');
end

if nargin < 4 | strcmp (datas, 'all')    
	datas =allfields (bag, inds, 'data');
end

if nargin < 5 | strcmp (waves, 'all')
    waves = allfields (bag, inds, 'wave');
end

for nind = 1:length (inds)        
   
    entry = struct ('anno', struct (), 'data', struct (), 'wave', struct ());
    entry = cpyfields (entry, bag(inds(nind)), annos, 'anno');
    entry = cpyfields (entry, bag(inds(nind)), datas, 'data');
    entry = cpyfields (entry, bag(inds(nind)), waves, 'wave');
    if isempty (subbag)
        subbag = entry;
    else
        subbag(end+1) = entry;
    end
end


function entry = cpyfields (entry, bag, fields, name)

for nanno = 1:length (fields)
    if (isfield (bag.(name), fields{nanno}))
        entry.(name).(fields{nanno}) = bag.(name).(fields{nanno});
    end
end


function fields = allfields (bag, inds, name)

fields = {};
for nind = 1:length (inds)
    for nanno = 1:length (bag(inds(nind)).(name))
        fields = [fields; fieldnames(bag(inds(nind)).(name))];
    end
end    
fields = unique (fields);  


function ise = iselem (value, set)
ise = any (strcmp (value, set));
