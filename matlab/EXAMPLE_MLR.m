clear all
close all

% global parameters

n_feat = 2;                               % #dimension
n_class = 4;                              % #classes
n_samp_train = 200;                        % #samples in samples_train set per class
n_samp_devel = 50;                        % #samples in samples_devel set per class

% models

ftrains = {@ssi_model_lda_train, @ssi_model_nb_train}; %, @ssi_model_knn_train};          % training functions
params_knn.k = 10;
params_nb.uselog = true;
params_lda = [];
ftrainparams = {params_lda, params_nb}; %, params_knn};                        % training parameters
fforwards = {@ssi_model_lda_forward, @ssi_model_nb_forward}; %, @ssi_model_knn_forward};   % forward functions

% create samples_train and samples_devel set

samples_train = zeros (n_samp_train * n_class, n_feat);
samples_devel = zeros (n_samp_devel * n_class, n_feat);
classes_train = zeros (n_samp_train * n_class, 1);
classes_devel = zeros (n_samp_devel * n_class, 1);
for i = 1:n_class
    for j = 1:n_feat
        r = rand;
%         r = i * 0.1;
        samples_train((i-1)*n_samp_train+1:i*n_samp_train,j) = random('Normal', r, 0.1, n_samp_train, 1);
        samples_devel((i-1)*n_samp_devel+1:i*n_samp_devel,j) = random('Normal', r, 0.1, n_samp_devel, 1);
    end
    classes_train((i-1)*n_samp_train+1:i*n_samp_train) = i;
    classes_devel((i-1)*n_samp_devel+1:i*n_samp_devel) = i;    
end

n_models = length (ftrains);
n_probs = cell (n_models, 1);
n_votes = cell (n_models, 1);
for i = 1:n_models
    
    % training and evaluation
    
    ftrain = ftrains{i};
    model = ftrain (samples_train, [], classes_train, [], ftrainparams{i});
    fforward = fforwards{i};
    [probs, votes] = fforward (model, samples_devel, []);
    [p, pc, cm] = ssi_eval_cm (classes_devel, votes, n_class)

    n_probs{i} = probs;
    n_votes{i} = votes;

end

classes_bits_train = zeros (n_samp_train * n_class, n_class);
for i = 1:n_class
    classes_bits_train(classes_train == i,i) = 1;
end

% perpare MLR

N = n_samp_train * n_class;
y_n = classes_train;
x_n = samples_train;
K = n_models;
P_kn = cell (K,1);
for k = 1:K
    P_kn{k} = zeros (N, n_class);
end

% do level-0 cross-validation

for n = 1:N
    
    L_minusj_x = samples_train;
    L_minusj_x(n,:) = [];
    L_minusj_y = classes_train;
    L_minusj_y(n) = [];
    L_j_x = samples_train(n,:);
    L_j_y = classes_train(n);
    
    for k = 1:K
        
        ftrain = ftrains{k};
        model = ftrain (L_minusj_x, [], L_minusj_y, [], ftrainparams{k});
        fforward = fforwards{k};
        [probs, votes] = fforward (model, L_j_x, []);
        P_kn{k}(n,:) = probs;
        
    end
    
end

% compute regression coefficients based on level-1 input

alpha = zeros (K, n_class);
X = zeros (N, K);

for i = 1:n_class

    y = y_n == i;    
    for k = 1:K
        X(:,k) = P_kn{k}(:,i);
    end
    alpha(:, i) = regress (y, X);

end

alpha

% train final level-0 models

models = cell(K,1);
for k = 1:K
    ftrain = ftrains{k};
    models{k} = ftrain (samples_train, [], classes_train, [], ftrainparams{k});    
end

% compute level-1 output using MLR

N = n_samp_devel*n_class;
mlr_probs = zeros (N, n_class);
Pkl_x = zeros (K, n_class);
for n = 1:N
   x = samples_devel(n,:);   
   for k = 1:K  
       fforward = fforwards{k};
       [probs, votes] = fforward (models{k}, x, []);
       Pkl_x(k,:) = probs;
   end
   mlr_probs(n,:) = sum (Pkl_x .* alpha, 1);
end
[xxx, mlr_votes] = max (mlr_probs, [], 2);
[p, pc, cm] = ssi_eval_cm (classes_devel, mlr_votes, n_class) 


% if possible visualize results

if n_feat == 2

    for i = 1:n_models
   
        subplot (1, n_models+1, i);
        hold on;
        scatter (samples_train(:,1),samples_train(:,2),3,classes_train,'filled')
        scatter (samples_devel(:,1),samples_devel(:,2),3,classes_devel,'filled')
        scatter (samples_devel(:,1),samples_devel(:,2),50,n_votes{i})

    end
    
    subplot (1, n_models+1, n_models+1);
            hold on;
        scatter (samples_train(:,1),samples_train(:,2),3,classes_train,'filled')
        scatter (samples_devel(:,1),samples_devel(:,2),3,classes_devel,'filled')
        scatter (samples_devel(:,1),samples_devel(:,2),50,mlr_votes)
    
end