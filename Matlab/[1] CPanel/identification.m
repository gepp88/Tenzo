clc

%% Load data

%a = load('ideTest1.mat')
a = load('errorTenzo.mat')




%% Prepare Data

timeTenzo = a.IdeDataTimeTenzo;
timeTenzoSec = timeTenzo*0.001;
timeVitruvio = a.IdeDataTimeVitruvio;
yIde = a.IdeDataRoll;
yIdeEst = a.IdeDataEstRoll;
uDelta = a.IdeDataDelta;
uBase = a.IdeDataThrottle;
uIdeM1 = uDelta + uBase;
uIdeM2 = - uDelta + uBase;
%% Plot Data

figure(11)
plot(a.IdeDataTimeTenzo,a.IdeDataEstRoll);

figure(13)
plot(a.IdeDataTimeVitruvio,a.IdeDataRoll);


figure(14)
plot(a.IdeDataRoll);

figure(12)
plot(a.IdeDataTimeTenzo,a.IdeDataDelta);

figure(15)
plot(uIdeM1,'r');
hold on 
plot(uIdeM2,'b');

%% Discrete Transfer function of motors
clear tf
Ts_motor = 0.009 ; %ms
%H = tf(0.0001195,[1 -0.9368],Ts_motor,'InputDelay',8)

%% Prepare simulated output fitting 85%

% Motor dynamics tf1 
H = load('estimation/motor87perc.mat')
Hd1 = c2d(H.tf1,Ts_motor)

% Motor dynamics with n4sid
H = load('estimation/discreteMotortf.mat')
H = H.mv
step(Hd1,'b');
hold on
step(H,'r');

% Simulate motor output
t = 0:Ts_motor:Ts_motor*(size(uIdeM1,1)-1);

[yM1,t,x] = lsim(Hd1,uIdeM1',t,0);
[yM2,t,x] = lsim(Hd1,uIdeM2',t,0);
[yM3,t,x] = lsim(H,uIdeM1',t);
[yM4,t,x] = lsim(H,uIdeM2',t);
hold on
subplot(2,1,1)
%plot(t,yM1,'b');
%hold on
%plot(t,yM2,'r');
%hold on
plot(t,yM3,'r');
hold on
plot(t,yM4,'b');
subplot(2,1,2)
plot(t,uIdeM1,'b');
hold on 
plot(t,uIdeM2,'r');

%% Convert to torque input signal

% Computing Thrust force. It is the result of vertical forces acting on all
% blade elements of one propeller
Radius = 0.115 % m
Ct = 0.11
rho = 1.2041 % kg/m^3
Aprop = pi*Radius^2

%MOTOR 3
% Convert to RPM
yM3_rpm= yM3*60
Thrust_newton_3 = Ct*rho*Aprop*(yM3_rpm).^2*Radius^2
Thrust_kg_3 = Thrust_newton_3/9.81


%MOTOR 4
% Convert to RPM
yM4_rpm= yM4*60
Thrust_newton_4 = Ct*rho*Aprop*(yM4_rpm).^2*Radius^2
Thrust_kg_4 = Thrust_newton_4/9.81

figure(20)
title('Thrust in kg')
plot(Thrust_kg_4,'r')
hold on 
plot(Thrust_kg_3,'b')
grid on
legend('Motor3','Motor4')

% Computing Torque
arm = 0.23 %m
Tau = arm*(Thrust_newton_4-Thrust_newton_3)


%% Preparing Training and Validation Sets

uOriginal = uIde(2:end);
yOriginal = yIde(2:end);
yEstOriginal = yIdeEst(2:end);

disp('Number of Samples');
disp(size(uIde,1))
disp(size(yIde,1))
disp(size(yIdeEst,1))

iter = ceil(size(uIde,1)*0.3);

uIde = uOriginal(1:iter);
yIde = yOriginal(1:iter);
yIdeEst = yEstOriginal(1:iter);


disp('Number of Samples');
disp(size(uIde,1))
disp(size(yIde,1))
disp(size(yIdeEst,1))

uVal = uOriginal(iter:end);
yVal = yOriginal(iter:end);
yEstVal = yEstOriginal(iter:end);

%% Computing deltaT for Tenzo and Vitruviano

%% Diff
plotDiff = 0;
dTtenzo = timeTenzo(2:end) - timeTenzo(1:end-1);
dTvitruviano = timeVitruvio(2:end) - timeVitruvio(1:end-1);

if (plotDiff) 
%     figure(3)
%     subplot(2,1,1);
%     % show results
%     title('dy/dt [rad/s^2]');
%     plot(dy./dt,'r');
%     subplot(2,1,2);
%     title('du/dt [us/s]');
%     plot(du./dt,'r');
end

disp('Ts from data')
dtt = mean(dTtenzo)
dtv = mean(dTvitruviano)
Ts = dtt;

%% Create time series Y-U data


% Create Id data
% zt  Training set
zt = iddata(yIde,uIde,Ts);
zt1 = iddata(yIdeEst,uIde,Ts);

% zv Validation set
zt1 = iddata(yIdeEst,uIde,Ts);

% Complete Set
z = iddata(y,z,Ts);

[ztDetrend, Ttest ]= detrend(zt,0);
[zt1Detrend, TtestTenzo ]= detrend(zt1,0);
[zvDetrend, Tvalidation ]= detrend(zv,0);
%[zDetrend, T ]= detrend(z,0);

delay = delayest(ztDetrend)
delay1 = delayest(zt1Detrend)
% estimated delay changes as a function of the model
delay4 = delayest(zt1Detrend,4,4)



%% 

V = arxstruc(ztDetrend,zvDetrend,struc(2,2,1:10));
[nn,Vm] = selstruc(V,0);

FIRModel = impulseest(ztDetrend);
clf
h = impulseplot(FIRModel);
showConfidence(h,3)
 
V = arxstruc(ztDetrend,zvDetrend,struc(1:10,1:10,delay));
nns = selstruc(V)

%% Identifiy linear discrete time model with arx

th2 = arx(ztDetrend,nns);
[ th4] = arx(ztDetrend,nns);
compare(zvDetrend(1:end),th2,th4);

%% Identifiy linear discrete time model with n4sid

[m, x0m] = n4sid(ztDetrend,2,'InputDelay',delay,'Ts',Ts);
[ms,x0] = n4sid(ztDetrend,1:10,'InputDelay',delay,'Ts',Ts);
figure
disp('Comparing ms and m')
compare(ztDetrend,ms,m)

%% Compare arx Vs n4Sid
disp('Comparing ms1 and ms')
compare(ztDetrend,ms,th2)
%% Manual compare

t = 0:Ts:Ts*size(yOriginal,1)-1;
[y,t,x] = lsim(ms,yOriginal,t,x0);
hold on
plot(t,zOriginal,'r');
hold on
plot(t,y,'b');

