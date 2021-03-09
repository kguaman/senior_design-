% Get Raw Data from sensor and get features every 2.56s
listOfTimers = timerfindall;
if ~isempty(listOfTimers)
    delete(listOfTimers(:));
end

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

clear all;

% Initializing variables and timer.
counter=1; % prevent extra raw data sample to be saved after calling 'getTD'
rawData = [];    
save('rawData.mat','rawData');
trainingData = [];
save('trainingData.mat','trainingData');
temp=[];    % temporarily stores extracted serial port output 
S = serialport('COM5',115200); % Connect Serial Port of Arduino to MATLAB
t = timer;
set(t,'executionMode','fixedRate');
set(t,'Period',1.1);
set(t,'StartDelay',1);
t.TimerFcn=@getFeatures;
start(t);
% Uncomment Below and in function 'getTD' to see count every 2.56s
count=1;


% Infinite loop until button is pressed.
while get(t,'RUNNING')=="on"    
data = string(readline(S)); % read serial port output
if strtrim(data)=="PRESSED" % if button is pressed, stop timer from running
    stop(t);
else
    temp = [str2double(split(data,','))]; % extract and store raw data from serial output 
    
%     Uncomment below and in 'getTD' to see raw data every 2.56s. Change
%     below to elseif counter == 1

    if counter == 2
        stop(t);

    elseif counter==1   
        rawData = [rawData;temp(1),temp(2),temp(3),temp(4),temp(5),temp(6)];   

    else 
        counter=1; % prevent saving another raw data sample during feature extraction
    end
    save('rawData.mat','rawData'); % save raw data sample
end
end

load trainingData.mat
disp(trainingData);
delete(t)