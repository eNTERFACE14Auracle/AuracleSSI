function [p, pc, cm] = ssi_eval_cm (prevotes, postvotes, n_classes)
% Calculates the classification rate by comparing each row of postlabels with
% the corresponding row of testlabels. If both class values are the same the 
% sample was correctly classified.
%
%  [p, pc, cm] = ssi_eval_cm (prevotes, postvotes)
%   
%   input:
%   prevotes:  ground truth
%   postvotes: evaluation result
%   n_classes: #classes
%
%   output:
%   p:        overall probability
%   pc:       average probability for each class
%   cm:       confussion matrix
%
%   2005, Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>

ssize = size (prevotes, 1);      % total size of data
csize = n_classes;         % total number of classes

missed = zeros (1, csize);                       % counter for missed classes
for i = 1:ssize
    if postvotes(i) ~= prevotes(i)            % if we are not in the expected class increment the missed-counter
        missed(prevotes(i)) = missed(prevotes(i)) + 1;
    end
end

cm = zeros (csize, csize + 1);           % confusion matrix 
for i = 1:ssize
    cm(prevotes(i), postvotes(i)) = cm(prevotes(i), postvotes(i)) + 1; % entry in confusion matrix
end
cm(:, csize + 1) = sum (cm, 2);


pc = zeros (1,csize);               % probability for each class
for i = 1:csize
    tmp = length (prevotes(prevotes == i));
    pc(i) = (tmp - missed(i)) / tmp; % calculate the probability for each class
end

p = mean (pc);                      % mean probability

