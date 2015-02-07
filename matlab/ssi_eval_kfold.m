function [prevotes, postprobs, postvotes] = ssi_eval_kfold (samples, dims, classes, users, ftrain, fforward, fparams, eparams)

ssize = size (samples, 1);  % total number of sampels
csize = max (classes);      % total number of classes
lsize = zeros (1, csize);   % total number of samples per class 
for i = 1:csize             
    lsize(i) = length (classes(classes == i));
end
kfolds = eparams.k;

postprobs = zeros (ssize, csize);

inds_per_class = cell(csize,1);
for c = 1:csize
    inds_per_class{c} = find (classes == c);
end

prevotes = [];
postprobs = [];
for k = 1:kfolds
   
    inds_in_fold = [];
    for c = 1:csize
        inds_in_fold = [inds_in_fold; inds_per_class{c}(k:kfolds:end)];
    end
    
    train_samples = samples;
    train_classes = classes;
    train_users = users;
    test_samples = train_samples(inds_in_fold, :);   
    train_samples(inds_in_fold, :) = [];     
    train_classes(inds_in_fold) = [];   
    if ~isempty (train_users)
        train_users(inds_in_fold) = [];
    end
    
    model = ftrain (train_samples, dims, train_classes, train_users, fparams);
    postprobs = [postprobs; fforward(model, test_samples, dims)];    
    prevotes = [prevotes; classes(inds_in_fold)];

end

[xxx, postvotes] = max (postprobs, [], 2);


