function model = ssi_model_lda_train (samples, dims, classes, users, params)
% Train linear discriminant function.
%
%  model = ssi_model_lda_train (samples, samples, classes, dims)
%   
%   input:
%   samples:  samples (one sample per row)
%   dims:     optional channel range ( samples(:,dims(1)+1:dims(2)) )
%   classes:  a column vector which assigns one class to each sample
%   users:    a column vector which assigns one user to each sample
%   params:   []
%
%   output:
%   model:    linear discrmiant function
%
%   2010, Johannes Wagner <go.joe@gmx.de>

if ~isempty (dims)
    samples = samples(:,dims(1)+1:dims(2));
end

ssize = size (samples, 1);         % total number of samples samples
fsize = size (samples, 2);         % total number of features
csize = max (classes);               % total number of classes
nsize = size (samples, 2);         % total number of features
lsize = zeros (1, csize);
for i = 1:csize
    lsize(i) = sum (classes == i);   % number of samples for each class
end
priors = lsize ./ ssize;            % prior probability

mc = zeros (csize, fsize);
for i = 1:csize                     % mean class feature vector
    mc(i, :) = mean (ssi_model_getclass (samples, classes, i), 1);
end
% mc = getClassCentres (samples, classes);

covmc = zeros (csize, nsize, nsize);
for i = 1:csize
    covmc(i,:,:) = cov (ssi_model_getclass (samples, classes, i), 1); % cov-matrix for each class
end
covm = zeros (nsize, nsize);        % mean cov-matrix for all classes
for i = 1:csize
    covm = covm + squeeze (covmc(i,:,:) .* priors(i)); 
end

model.mc = mc;
model.covm = covm;
model.priors = priors;
model.nclass = csize;
