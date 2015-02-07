function [probs, votes] = ssi_fusion_mlr_forward (model, samples, dims)
% Fusion strategy 'multiple linear regression'
%
%  probs = ssi_fusion_mlr_forward (model, samples, dims)
%   
%   input:
%   model:    fusion model
%   samples:  samples (one sample per row)
%   dims:     channel range ( nth channel: samples(:,dims(n)+1:dims(n+1)) )
%
%   output:
%   probs:    a matrix with the probability errors for each sample compared to each
%             class (the row indicates the sample and the column the class)
%   votes:    vector with winner classes
%
%   2010, Johannes Wagner <go.joe@gmx.de>

[n_samp, n_feat] = size (samples);
n_chann = length (dims) - 1;

n_class = model.n_class;
models = model.models;
fforward = model.fforward;
alphas = model.alphas;

probs = zeros (n_samp, n_class);
tmp_probs = zeros (n_chann, n_class);
for n = 1:n_samp   
   for k = 1:n_chann      
       tmp_probs(k,:) = fforward (models{k}, samples(n,:), dims(k:k+1));        
   end
   probs(n,:) = sum (tmp_probs .* alphas, 1);
end
[xxx, votes] = max (probs, [], 2);
