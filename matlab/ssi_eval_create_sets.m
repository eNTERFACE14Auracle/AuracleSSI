function [samples_train, classes_train, samples_devel, classes_devel, dims] = ssi_eval_create_sets (n_feat, n_class, n_chann, n_samp_train, n_samp_devel)
% Creates random train and devel sets
%
%  [samples_train, classes_train, samples_devel, classes_devel] =
%  ssi_eval_create_sets (n_feat, n_class, n_samp_train, n_samp_devel)
%   
%   input:
%   n_feat:         number of features
%   n_class:        number of classes
%   n_chann:        number of channels (drawn from same distribution)
%   n_samp_train:   number of train samples per class
%   n_samp_devel:   number of devel samples per class
%
%   output:
%   samples_train:  train samples
%   classes_train:  train classes
%   samples_devel:  devel samples
%   classes_devel:  devel classes
%   dims:           channel dimensions
%
%   2010, Johannes Wagner <go.joe@gmx.de>

samples_train = zeros (n_samp_train * n_class, n_feat * n_chann);
samples_devel = zeros (n_samp_devel * n_class, n_feat * n_chann);
classes_train = zeros (n_samp_train * n_class, 1);
classes_devel = zeros (n_samp_devel * n_class, 1);
for i = 1:n_class
    for j = 1:n_feat
        r = rand;
%         r = i * 0.1;
        samples_train((i-1)*n_samp_train+1:i*n_samp_train,j:n_feat:n_feat*n_chann) = random('Normal', r, 0.1, n_samp_train, n_chann);
        samples_devel((i-1)*n_samp_devel+1:i*n_samp_devel,j:n_feat:n_feat*n_chann) = random('Normal', r, 0.1, n_samp_devel, n_chann);
    end
    classes_train((i-1)*n_samp_train+1:i*n_samp_train) = i;
    classes_devel((i-1)*n_samp_devel+1:i*n_samp_devel) = i;    
end
dims = 0:n_feat:n_chann*n_feat;