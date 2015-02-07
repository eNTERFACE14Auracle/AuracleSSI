function model = ssi_fusion_cascade_train (samples, classes, params, dims)
% Fusion strategy 'cascading specialist'
%
%  model = ssi_model_knn_train (samples, samples, classes, dims)
%   
%   input:
%   samples:  samples (one sample per row)
%   classes:  a column vector which assigns one class to each sample
%   params:   ftrain = model trainig function 
%             fforward = model forward function
%             fparams = model params
%   dims:     channel range ( nth channel: samples(:,dims(n)+1:dims(n+1)) )
%
%   output:
%   model:    trained fusion model
%
%   2010, Johannes Wagner <go.joe@gmx.de>

slen = length (dims) - 1;
clen = max (classes);

ftrain = params.ftrain;
fforward = params.fforward;
fparams = params.fparams;

weights = zeros (slen, clen + 1);
for sn = 1:slen    
    channel = ssi_fusion_getchannel (samples, dims, sn);
    [models(sn), weights(sn,:)] = calcWeights (channel, classes, ftrain, fparams, fforward);    
end

%preparation ...

class_rating = zeros(1, clen);
for cn = 1:clen
    class_rating(cn) = sum (weights(:,cn)) / slen;
end

[xxx order] = sort (class_rating);

winner = zeros (1, clen);
for cn = 1:clen    
    [xxx winner(cn)] = max (weights(:,cn));        
end

%filler
[xxx, filler] = max(weights(:,clen+1));

model.fforward = fforward;
model.models = models;
model.filler = filler;
model.winner = winner;
model.order = order;
model.weights = weights;
model.nclass = clen;


function [model, weights] = calcWeights (samples, classes, ftrain, fparams, fforward)

n_class = max (classes);
model = ftrain (samples, classes, fparams);       
% faster
[probs, votes] = fforward (model, samples);   
[p, pc] = ssi_eval_cm (classes, votes, n_class);
% original implementation by florian
% [prevotes, postprobs, postvotes] = eval_loo (samples, dims, classes, ftrain, fforward, fparams);
% [p, pc] = eval_cm (prevotes, postvotes);
weights = [pc p];

