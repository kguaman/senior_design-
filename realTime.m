% Run for real time processing


listOfTimers = timerfindall;
if ~isempty(listOfTimers)
    delete(listOfTimers(:));
end

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

clear all;
load Mdl; 
counter=1; % prevent extra raw data sample to be saved after calling 'getTD'
rawData = [];    
save('rawData.mat','rawData');
realTimeData = [];
save('realTimeData.mat','realTimeData');
temp=[];    % temporarily stores extracted serial port output 

% Uncomment Below and in 'getTD' to see count every 2.56s
count=1;


%S = serialport('COM3',115200); % Connect Serial Port of Arduino to MATLAB
S= serial('COM5','BaudRate',115200);

fopen(S);
pause(3);


% Initalizing and starting Timer
t = timer;
set(t,'executionMode','fixedRate');
% set(t,'Period',2.56);
% set(t,'StartDelay',2.56);
set(t,'Period',.7);
set(t,'StartDelay',2);
t.TimerFcn=@realProcessing;
start(t);
% [Test, score] = predict(Mdl, trainingData_Exp1to3(:,1:14));%change

while get(t,'RUNNING')=="on"    % infinite loop until button pressed.
%data = string(readline(S)); % read serial port output
data=fscanf(S);
if strtrim(data)=="PRESSED" % if button is pressed, stop timer from running
    stop(t);
    fclose(S);
else
    temp = [str2double(split(data,','))]; % extract and store raw data from serial output 
    
    % Uncomment in 'getTD' to see raw data every 2.56s
    if counter == 2 
        stop(t);
        
    elseif counter==1  
        rawData = [rawData;temp(1),temp(2),temp(3),temp(4),temp(5),temp(6)];   

    else 
        if counter == 0
            [Test, score] = predict(Mdl, realTimeData(end,1:14));
            % disp(realTimeData(end,1:14)); Uncomment to see feature
            if Test(1) == 1
                disp('FALL');
                fprintf(S,"f");
                
            else
                disp('NON-FALL');
                fprintf(S,"n");
                
            end
            data=fscanf(S);
            while strtrim(data)~="f" && strtrim(data)~="n" 
                data=fscanf(S);
            end
            
            %pause(0.1);
            %data=fscanf(S);
            
% Uncomment Below for Send and Receive Debugging

%             disp("Arduino sent to MATLAB: "+ data);
%             if strtrim(data) == "f"
%                 disp("Arduino detected FALL");
%             elseif strtrim(data) == "n"
%                 disp("Arduino detected NON-FALL");
%             else
%                 disp("Timing incorrect");
%             end
        end
        counter=1;
    end
    save('rawData.mat','rawData'); 
end
end

% load realTimeData.mat
% disp(realTimeData);
delete(t)