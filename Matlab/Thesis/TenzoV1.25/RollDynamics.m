%% Roll Dynamics 22 mar 2017
% Giovanni Balestrieri 
% Info @ userk.co.uk

clc

%% Create 

Whovering = 1420;
Ts = 0.021;

% Loading identified motor dynamics
motorDynamics = load('discreteMotortf.mat')
motorDynamics = motorDynamics.mv

disp('Evaluating step response for motor dynamics. Press X');
pause()
opt = stepDataOptions('InputOffset',0,'StepAmplitude',1420);
step(motorDynamics,opt)
motorDynamics = d2d(motorDynamics,0.021)

% get numerator and denominator Roll
[motor_num_tf_discrete , motor_den_tf_discrete] = tfdata(motorDynamics,'v')

% Loading identified Roll dynamics

rollDynamics = load('discreteDynamicTenzo.mat');
rollDynamics = rollDynamics.mts

disp('Evaluating step response for roll dynamics. Press X');
pause()
step(rollDynamics)

%% Computing linearized 
% Computing Thrust force. It is the result of vertical forces acting on all
% blade elements of one propeller
Radius = 0.115 % m
Radius_in = 9 % in
Ct = 0.18
rho = 1.225 % kg/m^3
Aprop = pi*Radius^2

% Convert to RPM
Kforce = Ct*rho*Aprop*2*Whovering*Radius^2;
%Thrust_newton = rpm*Kforce;
%Thrust_kg = Thrust_newton/9.81



% get numerator and denominator Roll
[roll_num_tf_discrete , roll_den_tf_discrete] = tfdata(rollDynamics,'v')

open('testRollContSolo');


