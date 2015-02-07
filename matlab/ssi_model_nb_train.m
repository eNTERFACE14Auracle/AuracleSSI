function model = ssi_model_nb_train (samples, dims, classes, users, params)
% Train naive bayes classifier.
%
%  model = ssi_model_nb_train (samples, samples, classes, dims)
%   
%   input:
%   samples:  samples (one sample per row)
%   dims:     optional channel range ( samples(:,dims(1)+1:dims(2)) )
%   classes:  a column vector which assigns one class to each sample
%   users:  a column vector which assigns one user to each sample
%   params:   []
%
%   output:
%   model:    naive bayes classifier
%
%   2010, Johannes Wagner <go.joe@gmx.de>

if ~isempty (dims)
    samples = samples(:,dims(1)+1:dims(2));
end

if isempty (params)
    params{1} = true;
end

ssize = size (samples, 1);         % total number of samples samples
fsize = size (samples, 2);         % total number of features
csize = max (classes);             % total number of classes

lsize = zeros (csize, 1);
means = zeros (csize, fsize);
stds = zeros (csize, fsize);
for i = 1:csize
    lsize(i) = sum (classes == i);   % number of samples for each class
    means(i,:) = mean (samples(classes == i, :)); % means for each class
    stds(i,:) = std (samples (classes == i, :)); % stds for each lc
%     stds(i,:) = sqrt (sum (samples (classes == i, :).^2) ./ lsize(i) - means(i,:) .^2);
end
% stds (stds == 0) = eps;
priors = lsize ./ ssize;            % prior probability

model.uselog = params.uselog;
model.priors = priors;
model.means = means;
model.stds = stds;