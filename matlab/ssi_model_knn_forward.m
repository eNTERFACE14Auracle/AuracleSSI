function [probs, votes] = ssi_model_knn_forward (model, samples, dims)
% Computes the probability error for each sample compared to each class
% using a knn model.
%
%  probs = ssi_model_knn_forward (model, samples, dims)
%   
%   input:
%   model:    knn model
%   samples:  samples (one sample per row)
%   dims:     optional channel range ( samples(:,dims(1)+1:dims(2)) )
%
%   output:
%   probs:    a matrix with the probability errors for each sample compared to each
%             class (the row indicates the sample and the column the class)
%   votes:    vector with winner classes
%
%   2010, Johannes Wagner <go.joe@gmx.de>

if nargin > 2
    samples = samples(:,dims(1)+1:dims(2));
end

csize = max (model.classes);        % number of classes
ssize = size (samples, 1);

ds = dists (model.samples, samples);
[xxx, inds] = sort (ds);
inds = inds';
knearest = model.classes(inds(:,1:model.k));
if ssize == 1
    knearest = knearest';
end
probs = zeros (ssize, csize);
for i = 1:csize
    probs(:,i) = sum (knearest == i, 2) ./ model.k;
end
[xxx, votes] = max (probs, [], 2);


function ds = dists (A, B)

nA = size (A, 1);
nB = size (B, 1);

ds = (ones (nB, 1) * sum ((A.^2)', 1))' + ones(nA, 1) * sum ((B.^2)', 1) - 2.*(A*(B'));

% n2 = (ones(ncentres, 1) * sum((x.^2)', 1))' +  ones(ndata, 1) * sum((c.^2)',1) -  2.*(x*(c'));

ds(ds < 0) = 0;