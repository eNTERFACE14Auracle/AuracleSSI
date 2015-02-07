function [probs, votes] = ssi_model_nb_forward (model, samples, dims)
% Computes the probability error for each sample compared to each class
% using naive bayes classifier.
%
%  probs = ssi_model_nb_forward (model, samples, dims)
%   
%   input:
%   model:    linear discriminant function
%   samples:  samples (one sample per row)
%   dims:     optional channel range ( samples(:,dims(1)+1:dims(2)) )
%
%   output:
%   probs:    a matrix with the probability errors for each sample compared to each
%             class (the row indicates the sample and the column the class)
%   votes:    vector with winner classes
%
%   2010, Johannes Wagner <go.joe@gmx.de>

if ~isempty (dims)
    samples = samples(:,dims(1)+1:dims(2));
end

ssize = size (samples, 1);          % total number of samples
csize = length (model.priors);      % number of classes
fsize = size (samples, 2);          % number of features

probs = zeros (ssize, csize);       % now compute the propability errors for each sample
means = model.means;
stds = model.stds;
priors = model.priors;

if model.uselog
       
    for j = 1:csize                        
        valid = stds(j,:) ~= 0;
        p = repmat (nb_log (priors (j)), ssize, 1);
        s = repmat (stds(j,valid), ssize, 1);        
        s2 = s .^2;
        m = repmat (means(j,valid), ssize, 1);
        d = samples(:,valid) - m;
        probs(:,j) = exp ((p + sum (- nb_log (s) - d.^2 ./ (2 .* s2), 2)) ./ fsize);
    end

else  
    
    PIPI = sqrt (2 * 3.14159265358979323846);    
    stds(stds == 0) = realmin('single');
    for j = 1:csize                        
        p = repmat (priors(j), ssize, 1);
        m = repmat (means(j,:), ssize, 1);
        s = repmat (stds(j,:), ssize, 1);
        d = samples - m;
        probs(:,j) = p .* prod (((1 ./ (PIPI .* s)) .* exp (- ((d.^2) ./ (2 .* s)))), 2);            
    end

end

% normalize probs
% probs = probs ./ repmat (sum (probs, 2), 1, csize);
% probs = probs ./ repmat (max (probs, [], 2), 1, csize);
probs = mat2gray (probs);
% probs = probs - repmat (min (probs, [], 2), 1, csize);
% probs = probs ./ repmat (max (probs, [], 2), 1, csize);
% probs = probs ./ repmat (max (probs, [], 2), 1, csize);

[xxx, votes] = max (probs, [], 2);

function y = nb_log (x)

y = log (x); 
y (isinf(y)) = -46;

