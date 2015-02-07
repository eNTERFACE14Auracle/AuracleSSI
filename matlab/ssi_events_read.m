function events = ssi_events_read (filepath)
%
% read events from file
%
% [events] = ssi_events_read (filepath)
%
% input:
%   filepath                filepath
%
% output:
%   events                  events
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/18

if nargin < 1
    help ssi_events_read   
    error ('not enough arguments')
end

[pathstr, name, ext] = fileparts (filepath);
if isempty (ext)
    filepath = [filepath '.events'];
end

xDoc = xmlread (filepath);
xRoot = xDoc.getElementsByTagName ('events');
version = str2double (xRoot.item(0).getAttribute('ssi-v').toCharArray);

xEvents = xDoc.getElementsByTagName ('event');
nEvents = xEvents.getLength ();
events = struct ('sender', [], 'event', [], 'from', 0, 'dur', 0, 'prob', 0, 'type', [], 'state', 0, 'glue', 0, 'data', []); 
for i=0:nEvents-1
    e = xEvents.item(i);
    events(i+1).sender = e.getAttribute('sender').toCharArray';
    events(i+1).event = e.getAttribute('event').toCharArray';
    events(i+1).from = str2double (e.getAttribute('from').toCharArray);
    events(i+1).dur = str2double (e.getAttribute('dur').toCharArray);
    events(i+1).prob = str2double (e.getAttribute('prob').toCharArray);
    events(i+1).type = e.getAttribute('type').toCharArray';
    switch e.getAttribute('state').toCharArray;
        case 'completed'
            state = 0;
        otherwise
            state = 1;
    end
    events(i+1).state = state;
    events(i+1).glue = str2double (e.getAttribute('glue').toCharArray);
    events(i+1).data = [];
    switch events(i+1).type        
        case 'STRING'
            events(i+1).data = e.getTextContent.toCharArray';
        otherwise
            warning (['type ', events(i+1).type, ' not supported']);
    end
end



