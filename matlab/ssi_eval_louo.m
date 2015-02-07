function [prevotes, postprobs, postvotes] = ssi_eval_louo (samples, dims, classes, users, ftrain, fforward, fparams, eparams)

uids = unique(users);       % user ids
usize = length (uids);      % total number of users
ssize = size (samples, 1);  % total number of sampels
csize = max (classes);      % total number of classes
lsize = zeros (1, csize);   % total number of samples per class 
for i = 1:csize             
    lsize(i) = length (classes(classes == i));
end

postprobs = [];
prevotes = [];

for i = 1:usize
    
    train_samples = samples;
    train_classes = classes;
    train_users = users;
    test_sample = train_samples(users == uids(i), :);     % take all samples of a user from trainings-data
    test_classes = train_classes(users == uids(i));       
    test_users = train_users(users == uids(i));
    train_samples(users == uids(i), :) = [];              % delete these samples from trainings-data
    train_classes(users == uids(i)) = [];      
    train_users(users == uids(i)) = [];
    
    model = ftrain (train_samples, dims, train_classes, train_users, fparams);
    ps = fforward (model, test_sample, dims);    
    
    postprobs = [postprobs; ps];
    prevotes = [prevotes; test_classes];

end

[xxx, postvotes] = max (postprobs, [], 2);


