% Feature Extraction for Real Time Data

function [rawData] = realProcessing(~,~)
load rawData.mat

% Uncomment below and in 'getRawData' to see count every 2.56s
% c=evalin('base','count');
% disp(c);
% c=c+1;
% assignin('base','count',c);

dimension = size(rawData);
row= dimension(1);   % get rawData row dimension
column=dimension(2); % get rawData column dimension

% Reset values
accelX_sum=0;
accelY_sum=0;
accelZ_sum=0;
gyroX_sum=0;
gyroY_sum=0;
gyroZ_sum=0;
accelMag_sum=0;
gyroMag_sum=0;

% Sum of accel and gyro individual axis
for sampleLine=1:row
    temp=[rawData(sampleLine,1:column)];
    accelX_sum = accelX_sum + temp(1,1);
    accelY_sum = accelY_sum + temp(1,2);
    accelZ_sum = accelZ_sum + temp(1,3);
    gyroX_sum = gyroX_sum + temp(1,4);
    gyroY_sum = gyroY_sum + temp(1,5);
    gyroZ_sum = gyroZ_sum + temp(1,6);
    accelMag_sum = accelMag_sum + sqrt((temp(1,1)^2) + (temp(1,2)^2) + ...
        (temp(1,3)^2));
    gyroMag_sum = gyroMag_sum + sqrt((temp(1,4)^2) + (temp(1,5)^2) + ...
        (temp(1,6)^2));
end

% Get Mean Accel and Gyro in x,y,z directions
accelX_mean = accelX_sum/row;
accelY_mean = accelY_sum/row;
accelZ_mean = accelZ_sum/row;
gyroX_mean = gyroX_sum/row;
gyroY_mean = gyroY_sum/row;
gyroZ_mean = gyroZ_sum/row;

% Get Mean Accel and Gyro Magnitude
accelMag_mean = accelMag_sum/row;
gyroMag_mean = gyroMag_sum/row;

% Get RMS Accel and Gyro in x,y,z directions
accelX_rms = rms(rawData(1:row,1));
accelY_rms = rms(rawData(1:row,2));
accelZ_rms = rms(rawData(1:row,3));
gyroX_rms = rms(rawData(1:row,4));
gyroY_rms = rms(rawData(1:row,5));
gyroZ_rms = rms(rawData(1:row,6));


% save realTimeData in workspace
rtd=[accelX_mean, accelY_mean, accelZ_mean, gyroX_mean, gyroY_mean, ...
    gyroZ_mean, accelMag_mean, gyroMag_mean, accelX_rms, accelY_rms, ...
    accelZ_rms, gyroX_rms, gyroY_rms, gyroZ_rms];
% disp(rtd);
filename = 'realTimeData.mat';
m = matfile(filename,'Writable', true);
realTimeData = m.realTimeData;
realTimeData = [realTimeData;rtd];
save('realTimeData.mat','realTimeData');
assignin('base','realTimeData',realTimeData);
clear m;



% Uncomment below to analyze rawData and realTimeData (verify results).
% Comment last two lines before end.

% rtd_size=size(realTimeData);
% rtd_row=rtd_size(1);
% disp(rtd_row);
% if rtd_row ~= 3
%     assignin('base','rawData',[]);
%     assignin('base','counter',0);
% else 
%     assignin('base','counter',2);
% end


assignin('base','rawData',[]);
assignin('base','counter',0);

end

