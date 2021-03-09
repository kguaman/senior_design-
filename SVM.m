load test_train_data

% Training Data (:,1:14) are the features, Exp1(:,15) is the class
Mdl = fitcsvm(test_train_data(:,1:14),test_train_data(:,15) ...
            ,'Standardize',true,'KernelFunction','RBF','KernelScale', ...
            'auto','OutlierFraction',0.05);
save Mdl Mdl

[Test,score] = predict(Mdl, test_train_data(:,1:14));
PerformanceChest= 100-norm([Test-test_train_data(:,15)]).^2/length(test_train_data(:,15))*100
con_matrix = confusionchart(test_train_data(:,15),Test);

