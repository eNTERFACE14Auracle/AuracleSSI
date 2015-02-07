clear all
close all

% global parameters

n_feat = 2;                               % #dimension
n_class = 4;                              % #classes
n_chann = 2;                              % #channels
n_samp_train = 200;                       % #samples in samples_train set per class
n_samp_devel = 50;                        % #samples in samples_devel set per class

% model

fmtrain = @ssi_model_nb_train;            % model training function
fmparams.uselog = true;              % model training parameters
fmforward = @ssi_model_nb_forward;        % model forward function
fmeval = @ssi_eval_kfold;

% fusion

ftrain = @ssi_fusion_mlr_train;
fparams.ftrain = fmtrain;
fparams.fparams = fmparams;
fparams.fforward = fmforward;
fparams.feval = fmeval;
fparams.eparams.k = 5;
fforward = @ssi_fusion_mlr_forward;

% ftrain = @ssi_fusion_cascade_train;
% fparams.ftrain = fmtrain;
% fparams.fparams = fmparams;
% fparams.fforward = fmforward;
% fforward = @ssi_fusion_cascade_forward;

% create samples_train and samples_devel set

[samples_train, classes_train, samples_devel, classes_devel, dims] = ssi_eval_create_sets (n_feat, n_class, n_chann, n_samp_train, n_samp_devel);

% training and evaluation

model = ftrain (samples_train, dims, classes_train, [], fparams);
[probs, votes] = fforward (model, samples_devel, dims);
[p, pc, cm] = ssi_eval_cm (classes_devel, votes, n_class)

% visualize results

if n_feat == 2

    hold on;
    scatter (samples_train(:,1),samples_train(:,2),3,classes_train,'filled')
    scatter (samples_devel(:,1),samples_devel(:,2),3,classes_devel,'filled')
    scatter (samples_devel(:,1),samples_devel(:,2),50,votes)

end