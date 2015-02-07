function [prevotes, postprobs, postvotes] = ssi_eval_loo (samples, dims, classes, users, ftrain, fforward, fparams, eparams)

ssize = size (samples, 1);  % total number of sampels
csize = max (classes);      % total number of classes
lsize = zeros (1, csize);   % total number of samples per class 
for i = 1:csize             
    lsize(i) = length (classes(classes == i));
end

postprobs = zeros (ssize, csize);

for i = 1:ssize
    
    train_samples = samples;
    train_classes = classes;
    train_users = users;
    test_sample = train_samples (i, :);    % take a sample from trainings-data
    train_samples(i, :) = [];        % delete this sample from trainings-data
    train_classes(i) = [];         % and from group-vector
    train_users(i) = [];
    
    model = ftrain (train_samples, dims, train_classes, train_users, fparams);
    postprobs(i,:) = fforward (model, test_sample, dims);    

end

[xxx, postvotes] = max (postprobs, [], 2);
prevotes = classes;

