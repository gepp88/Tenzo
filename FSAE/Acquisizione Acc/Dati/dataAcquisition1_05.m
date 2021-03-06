%% Thor - FormulaSAE
% - Data acquisition
% - Filtering
% - Angle Estimation from acc values TODO

%% Variable declaration
% %3.3 V
% xRawMin = 416;
% xRawMax = 250;
%  
% yRawMin = 400;
% yRawMax = 268;
%  
% zRawMin = 401;
% zRawMax = 269;

% 5 V
xRawMin = 628;
xRawMax = 404;
 
yRawMin = 619;
yRawMax = 398;
 
zRawMin = 626;
zRawMax = 426;

RAD_TO_DEG = 180/pi;

%% Data acquisition

fid = fopen('DATA0001.csv');
M = textscan(fid,'%f%f%s','delimiter',',');
disp('Data acquisition completed.');
fclose(fid);

%Time M{1}; % Values M{2}; % Type M{3};

numberOfSamples = size(M{1},1);
% Detect acc type
contX = 1;
contY = 1;
contZ = 1;
for i=1:numberOfSamples
    % Save Xaxis' values
    if strcmp(M{3}(i,1),'ACC1X')
        accXIndex(1,contX)=i;
        contX = contX+1;
        %M{3}(i,1)
    end
    
    % Save Yaxis' values
    if strcmp(M{3}(i,1),'ACC1Y')
        accYIndex(1,contY)=i;
        contY= contY+1;
        %M{3}(i,1)
    end
    
    % Save Zaxis' values
    if strcmp(M{3}(i,1),'ACC1Z')
        accZIndex(1,contZ)=i;
        contZ = contZ + 1;
        %M{3}(i,1)
    end
end
numberOfAccSamples = size(accXIndex,2);
disp('Number of samples');
numberOfAccSamples

for j=1:(numberOfAccSamples-1)
    accXRaw(1,j) = M{2}(accXIndex(1,j),1);
    accYRaw(1,j) = M{2}(accYIndex(1,j),1);
    accZRaw(1,j) = M{2}(accZIndex(1,j),1);
    time(1,j) = M{1}(accXIndex(1,j),1);
end

disp('Press X, to plot Raw values');
pause();
%% Plot Raw Values

% X Axis
figure(1)
plot(time,accXRaw,'--rs','LineWidth',2,...
                'MarkerEdgeColor','k',...
                'MarkerFaceColor','g',...
                'MarkerSize',10);
title('Raw X Acc');
xlabel('Acc');
ylabel('Time [ds]');

% Y Axis
figure;
plot(time,accYRaw,'--rs','LineWidth',2,...
                'MarkerEdgeColor','k',...
                'MarkerFaceColor','g',...
                'MarkerSize',10);
title('Raw Y Acc');
xlabel('Acc');
ylabel('Time [ds]');

% Z Axis
figure;
plot(time,accZRaw,'--rs','LineWidth',2,...
                'MarkerEdgeColor','k',...
                'MarkerFaceColor','g',...
                'MarkerSize',10);
title('Raw Z Acc');
xlabel('Acc');
ylabel('Time [ds]');


disp('Press X, to convert Raw values in Acceleration values in G');
pause();
%% Convert to real acceleration

% X Axis
for i=1:numberOfAccSamples-1
    accReal(1,i)=(accXRaw(1,i) - xRawMin) * (xRawMax - xRawMin) / (1000 + 1000) + xRawMin;
    accReal(1,i)=accReal(1,i)/1000;
end

% Y Axis
for i=1:numberOfAccSamples-1
    accReal(2,i)=(accYRaw(1,i) - yRawMin) * (yRawMax - yRawMin) / (1000 + 1000) + yRawMin;
    accReal(2,i)=accReal(2,i)/1000;
end

% Z Axis
for i=1:numberOfAccSamples-1
    accReal(3,i)=(accZRaw(1,i) - zRawMin) * (zRawMax - zRawMin) / (1000 + 1000) + zRawMin;
    accReal(3,i)=accReal(3,i)/1000;
end
plot(time,accReal(1,:),'--rs','LineWidth',2,...
                'MarkerEdgeColor','k',...
                'MarkerFaceColor','g',...
                'MarkerSize',10);
title('X Acc');
xlabel('Acc');
ylabel('Time [ds]');
figure
plot(time,accReal(2,:),'--rs','LineWidth',2,...
                'MarkerEdgeColor','k',...
                'MarkerFaceColor','g',...
                'MarkerSize',10);
figure
plot(time,accReal(3,:),'--rs','LineWidth',2,...
                'MarkerEdgeColor','k',...
                'MarkerFaceColor','g',...
                'MarkerSize',10);
            
            
disp('Press X for spectral analysis');
pause();
%% Spectral analysis

% Plots magnitude spectrum of the signal

% X Axis
mags(1,:)=abs(fft(accReal(1,:)));
figure(3)
plot(mags(1,:));
xlabel('DFT Bins');
ylabel('Magnitude');

% Plots first half of DFT (normalized frequency)
figure(3)
num_bins = length(mags(1,:));
% plot([0:1/(num_bins/2 -1):1],X_mags(1:num_bins/2))
% xlabel('Normalized frequency [\pi rads/samples]');
% ylabel('Magnitude');

% Y Axis
mags(2,:)=abs(fft(accReal(2,:)));
% figure(3)
% plot(X_mags);
% xlabel('DFT Bins');
% ylabel('Magnitude');

% Plots first half of DFT (normalized frequency)
figure(4)
num_bins = length(mags(2,:));
% plot([0:1/(num_bins/2 -1):1],X_mags(1:num_bins/2))
% xlabel('Normalized frequency [\pi rads/samples]');
% ylabel('Magnitude');

% Z Axis
mags(3,:)=abs(fft(accReal(3,:)));
% figure(3)
% plot(X_mags);
% xlabel('DFT Bins');
% ylabel('Magnitude');

% Plots first half of DFT (normalized frequency)
figure(5)
num_bins = length(mags(3,:));
% plot([0:1/(num_bins/2 -1):1],X_mags(1:num_bins/2))
% xlabel('Normalized frequency [\pi rads/samples]');
% ylabel('Magnitude');


%% Filter Design 1
disp('Filter design 1 butterworth 3th order cf:4 Hz ');
% Designs a third order butterworth filter 
% plots frequency response (normalized frequency)
[b a] = butter(3,0.10,'low');
H = freqz(b,a,floor(num_bins/2));
figure
hold on
plot([0:1/(num_bins/2 -1):1], abs(H), 'r');
title('Cutting frequency');

% Filters the signal X
Accfiltered(1,:) = filter(b,a,accReal(1,:));
Accfiltered(2,:) = filter(b,a,accReal(2,:));
Accfiltered(3,:) = filter(b,a,accReal(3,:));

% Plots the filtered signal
figure(3)
plot(Accfiltered(1,:),'r')
title('Filtered Signal - Second Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

% Plots the filtered signal
figure(4)
plot(Accfiltered(2,:),'r')
title('Filtered Signal - Second Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

% Plots the filtered signal
figure(5)
plot(Accfiltered(3,:),'r')
title('Filtered Signal - Second Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

disp('Press X');
pause();
%% Filter Design 2 

disp('Filter design 2 butterworth 3th order cf: 15HZ');
% Redesign the filter using a different cutting freq
[b2 a2] = butter(3,0.15,'low');
% Plots the magnitude spectrum and compare with lower order
H2 = freqz(b2,a2,floor(num_bins/2));
figure;
hold on
plot([0:1/(num_bins/2 -1):1], abs(H2), 'r');
%filter the noisy signal
Accfiltered2(1,:) = filter(b2,a2,accReal(1,:));
Accfiltered2(2,:) = filter(b2,a2,accReal(2,:));
Accfiltered2(3,:) = filter(b2,a2,accReal(3,:));


% X Axis
figure(3)
plot(Accfiltered2(1,:),'g');
title('Filtered Signal - 20 Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

% Y Axis
figure(4)
plot(Accfiltered2(2,:),'g');
title('Filtered Signal - 20 Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

% Z Axis
figure(5)
plot(Accfiltered2(3,:),'g');
title('Filtered Signal - 20 Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

disp('Press X');
pause();
%% Filter Design 3 

% Redesign the filter using a different cutting freq
[b3 a3] = butter(4,0.2,'low');
% Plots the magnitude spectrum and compare with lower order
H2 = freqz(b2,a2,floor(num_bins/2));
figure;
hold on
plot([0:1/(num_bins/2 -1):1], abs(H2), 'r');
%filter the noisy signald
Accfiltered3(1,:) = filter(b3,a3,accReal(1,:));
Accfiltered3(2,:) = filter(b3,a3,accReal(2,:));
Accfiltered3(3,:) = filter(b3,a3,accReal(3,:));

% X Axis
figure(3)
plot(Accfiltered3(1,:),'g');
title('Filtered Signal - 20 Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

% X Axis
figure(4)
plot(Accfiltered3(2,:),'g');
title('Filtered Signal - 20 Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

% Z Axis
figure(5)
plot(Accfiltered3(3,:),'g');
title('Filtered Signal - 20 Order Butterworth');
xlabel('Samples');
ylabel('Amplitude');

disp('Press X');
pause();
%% Comparison between filters: Digital Arduino Vs Matlab

figure(7)
% Plots Raw acc data
plot(time,accReal(1,:),'k');
hold on
% Arduino digital filter
%plot(dat.time,dat.y,'b','LineWidth',2);
%hold on
% Matlab filter
plot(time,Accfiltered(1,:),'r','LineWidth',2);
hold on
% Plots cF = 0.10 Matlab filtered, order 3
plot(time,Accfiltered2(1,:),'g-','LineWidth',2);
hold on
% Plots cf = 0.3 Matlab filtered order 30
plot(time,Accfiltered3(1,:),'c-','LineWidth',2);
title('X Axis - Filter Comparison');
xlabel('Samples');
ylabel('Amplitude [m*s^-2]');
grid on
grid minor

figure(8)
% Plots Raw acc data
plot(time,accReal(2,:),'k');
hold on
% Arduino digital filter
%plot(dat.time,dat.y,'b','LineWidth',2);
%hold on
% Matlab filter
plot(time,Accfiltered(2,:),'r','LineWidth',2);
hold on
% Plots cF = 0.10 Matlab filtered, order 3
plot(time,Accfiltered2(2,:),'g-','LineWidth',2);
hold on
% Plots cf = 0.3 Matlab filtered order 30
plot(time,Accfiltered3(2,:),'c-','LineWidth',2);
title('Y Axis - Filter Comparison');
xlabel('Samples');
ylabel('Amplitude [m*s^-2]');
grid on
grid minor

figure(9)
% Plots Raw acc data
plot(time,accReal(3,:),'k');
hold on
% Arduino digital filter
%plot(dat.time,dat.y,'b','LineWidth',2);
%hold on
% Matlab filter
plot(time,Accfiltered(3,:),'r','LineWidth',2);
hold on
% Plots cF = 0.10 Matlab filtered, order 3
plot(time,Accfiltered2(3,:),'g-','LineWidth',2);
hold on
% Plots cf = 0.3 Matlab filtered order 30
plot(time,Accfiltered3(3,:),'c-','LineWidth',2);
title('Z Axis - Filter Comparison');
xlabel('Samples');
ylabel('Amplitude [m*s^-2]');
grid on
grid minor

disp('Filtered values are stored in Accfiltered3(i,:) for i=1=xAxis, i=2=yAxis, i=3=zAxis');

%% Angle estimation 
clc;
disp('Angle estimation: Roll and Pitch');

% TODO - Fix signs
for i=1:numberOfAccSamples-1
    angleAcc(1:i) = (atan2(Accfiltered3(1,i),Accfiltered3(3,i))) * RAD_TO_DEG;
    angleAcc(2,i) = (atan2(Accfiltered3(2,i),Accfiltered3(3,i))) * RAD_TO_DEG; 
end
plot(time,angleAcc(1,:));
figure
plot(time,angleAcc(2,:));

