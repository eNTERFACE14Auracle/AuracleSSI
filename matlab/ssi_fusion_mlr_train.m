function model = ssi_fusion_mlr_train (samples, dims, classes, users, params)
% Fusion strategy 'multiple linear regression'
%
%  model = ssi_fusion_mlr_train (samples, samples, classes, dims)
%   
%   input:
%   samples:  samples (one sample per row)
%   dims:     channel range ( nth channel: samples(:,dims(n)+1:dims(n+1)) )
%   classes:  a column vector which assigns one class to each sample
%   users:    a column vector which assigns one user to each sample
%   params:   ftrain = model trainig function 
%             fforward = model forward function
%             fparams = model params
%
%   output:
%   model:    trained fusion model
%
%   2010, Johannes Wagner <go.joe@gmx.de>

n_class = max (classes);
[n_samp, n_feat] = size (samples);
n_chann = length (dims) - 1;

ftrain = params.ftrain;
fforward = params.fforward;
fparams = params.fparams;
feval = params.feval;
eparams = params.eparams;

% perpare MLR

p_chann = cell (n_chann,1);
for k = 1:n_chann
    p_chann{k} = zeros (n_samp, n_class);
end

% do level-0 cross-validation

 for k = 1:n_chann
    [prevotes, postprobs, postvotes] = feval (samples, dims(k:k+1), classes, users, ftrain, fforward, fparams, eparams);
    p_chann{k} = postprobs;
 end

% compute regression coefficients based on level-1 input

alphas = zeros (n_chann, n_class);
X = zeros (n_samp, n_chann);

for i = 1:n_class

    y = classes == i;    
    for k = 1:n_chann
        X(:,k) = p_chann{k}(:,i);
    end
    alphas(:, i) = regress (y, X);

end

% norm alphas?
alphas = alphas ./ repmat (sum (alphas, 1), n_chann, 1);

model.alphas = alphas;

% train final level-0 models

models = cell(n_chann,1);
for k = 1:n_chann
    models{k} = ftrain (samples, dims(k:k+1), classes, users, fparams);    
end

model.models = models;
model.fforward = fforward;
model.n_class = n_class;
