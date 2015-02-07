clear all

% global parameters

n_feat = 2;                               % #dimension
n_class = 4;                              % #classes
n_samp_train = 50;                        % #samples in samples_train set per class
n_samp_devel = 10;                        % #samples in samples_devel set per class

% nb

% ftrain = @ssi_model_nb_train;          % training function
% fparams.uselog = true;            % training parameters
% fforward = @ssi_model_nb_forward;      % forward function

% lda

ftrain = @ssi_model_lda_train;          % training function
fparams = [];                      % training parameters
fforward = @ssi_model_lda_forward;      % forward function

% knn

% ftrain = @ssi_model_knn_train;            % training function
% fparams.k = 10;                      % training parameters
% fforward = @ssi_model_knn_forward;        % forward function

% create samples_train and samples_devel set

[samples_train, classes_train, samples_devel, classes_devel] = ssi_eval_create_sets (n_feat, n_class, 1, n_samp_train, n_samp_devel);

% training and evaluation

model = ftrain (samples_train, [], classes_train, [], fparams);
[probs, votes] = fforward (model, samples_devel, []);
[p, pc, cm] = ssi_eval_cm (classes_devel, votes, n_class)

% visualize results

if n_feat == 2

    hold on;
    scatter (samples_train(:,1),samples_train(:,2),3,classes_train,'filled')
    scatter (samples_devel(:,1),samples_devel(:,2),3,classes_devel,'filled')
    scatter (samples_devel(:,1),samples_devel(:,2),50,votes)

end