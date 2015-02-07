function [probs, votes] = ssi_model_lda_forward (model, samples, dims)
% Computes the probability error for each sample compared to each class
% using the linear discriminant function.
%
%  probs = ssi_model_lda_forward (model, samples, dims)
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
csize = model.nclass;

probs = zeros (ssize, csize);           % now compute the propability errors for each sample
mc = model.mc;
covm = model.covm;
priors = model.priors;
for i = 1:ssize
    for j = 1:csize
        tmp = samples(i,:) - mc(j,:);
%         probs(i,j) = - tmp * inv (covm) * tmp' + 2 * log (priors(j));
        probs(i,j) = - tmp * pinv (covm) * tmp' + 2 * log (priors(j)); % use pseudeinverse matrix instead
    end
end

% normalize probs
% probs = probs + repmat (max (probs,[],2) - min (probs,[],2), 1, csize);
% probs = probs ./ repmat (sum (probs, 2), 1, csize);
probs = mat2gray (probs);

[xxx, votes] = max (probs, [], 2);