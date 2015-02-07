function [y, hist] = ssi_transform_mvgavg (x, sr, dlen, params, hist)
%
% moving average
%
% [y, hist] = ssi_transform_mvgavg (x, sr, dlen, params, hist = [])
%
% input:
%   x                       input signal
%   sr                   	sample rate
%   dlen                    #delta samples
%   params                  ws = window size in s (10)
%                           (if ws is vector moving average
%                           is calculated for each window size)
%   hist                    history
%
% output:
%   y                       output series
%   hist                    history for recursive call
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
%

if nargin < 3
    help ssi_transform_mvgavg
    error ('not enough arguments')
end

if nargin < 4 || isempty (params)
    params.ws = 5;
end

% read params
ws = params.ws;

% x size
[len, dim] = size (x);
Ns = round (sr * ws);
number = length (Ns);
N_max = max (Ns);

% prepare x to get rid of loops
Ns = repmat (Ns(:)', 1, dim);
inds = repmat (1:dim, number, 1);
x = x(:,inds(:));

if nargin < 5 || isempty (hist)
    
    y = x(1,:);
    % store last samples
    hist.last = repmat (y, N_max+1, 1); 
    % store history pointer
    hist.head = N_max;
    % store sum
	hist.sum = Ns .* y;   
end

% allocate result matrix
y = zeros (len, dim*number);

% retrieve history from last call
head = hist.head;
last = hist.last;
sumval = hist.sum;

for i = 1:len
    
    % increment history
    head = hinc (head, N_max);        
    
    % insert new sample in history
    x_0 = x(i,:);
    last(head,:) = x_0;   
    
    % retreive last sample of history
    % the correct position depends on N
    % hpos gives us the correct position for each column
    x_N = last(hpos (head, N_max, Ns) + (0:(dim*number)-1) .* (N_max+1));
    
    % update sum and squared sum
	sumval = sumval - x_N + x_0; 

    % calculate average/variance and store result
    y(i,:) = sumval ./ Ns;
end

% store history for next call
hist.head = head;
hist.last = last;
hist.sum = sumval;



% increments history head position
function pos = hinc (head, N_max)

pos = head + 1;
if pos > N_max+1
    pos = 1;
end


% returns history position relative to head
% pos: points to head
% N: length of history
% offs: distances from head
function pos = hpos (head, N_max, offs)

pos = head - offs;
ind = pos <= 0;
pos(ind) = N_max + 1 + pos(ind);







