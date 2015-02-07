function model = ssi_model_knn_train (samples, classes, params, dims)
% Train knn classifier.
%
%  model = ssi_model_knn_train (samples, samples, labels, dims)
%   
%   input:
%   samples:  samples (one sample per row)
%   classes:  a column vector which assigns one class to each sample
%   params:   k = #neighbors
%   dims:     optional channel range ( samples(:,dims(1)+1:dims(2)) )
%
%   output:
%   model:   knn classifier
%
%   2010, Johannes Wagner <go.joe@gmx.de>

if nargin > 3
    samples = samples(:,dims(1)+1:dims(2));
end

model.samples = samples;
model.classes = classes;
model.k = params.k;
