function [events, labels] = ssi_signal_readevents (filename)
%
% reads events from a file
%
% [events, labels] = ssi_signal_readevents (filename)
%
% input:
%   filename                filename
%
% output:
%   events                  vector with end and stop times of each event (in seconds)
%   labels                  vector with label names (if found)
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2010/02/15

if nargin < 1
    help ssi_signal_readevents;
    return;
end

fid = fopen (filename);
           
events = [];
labels = {};
counter = 1;
while 1
    rem = fgetl(fid);
    if ~ischar(rem)
        break
    end
    if isempty (rem)
        continue;
    end
    [tok, rem] = strtok (rem);
    events(counter,1) = str2double (tok);
    [tok, rem] = strtok (rem);
    events(counter,2) = str2double (tok);
    [tok, rem] = strtok (rem);
    labels{counter,1} = tok;
    counter = counter + 1;
end

fclose (fid);