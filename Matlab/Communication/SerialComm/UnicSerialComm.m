%% Serial communication between Matlab and Xbee module connected to Arduino
%  Giovanni Balestrieri Aka UserK - 31/12/2013

global Roll;
global theta;
global thetaOld;
global Pitch;
global pitch;
global Yaw;
global yaw;
global OmegaX;
global wx;
global OmegaY;
global wy ;
global OmegaZ;
global wz; 
global AccX;
global ax; 
global AccY;
global ay;
global AccZ;
global az;
global alpha;
read = true;
%% Check to see if there are existing serial objects (instrfind) whos 'Port' property is set to 'COM3'
oldSerial = instrfind('Port', '/dev/ttyS0'); 
% can also use instrfind() with no input arguments to find ALL existing serial objects
if (~isempty(oldSerial))  % if the set of such objects is not(~) empty
    disp('WARNING:  ttyS0 in use.  Closing.')
    fclose(oldSerial)
    delete(oldSerial)
    clear oldSerial
end

%%  Setting up serial communication
% XBee expects the end of commands to be delineated by a carriage return.
xbee = serial('/dev/ttyS0','baudrate',9600,'terminator','CR','tag','Quad');

% Max wait time
set(xbee, 'TimeOut', 5);  
% One message long buffer
set(xbee, 'InputBufferSize',  390 )
% Open the serial
fopen(xbee);

%% Testing Wireless communication

fprintf(xbee,'T');
ack = fscanf( xbee, '%s')
if (strcmp(deblank(ack), 'K0') == 1)
    yes = [11];
    fwrite(xbee,yes);
    disp ('Ok');
else
    no =[0];
    fwrite(xbee,no);
    disp ('No');
end

%% IF THERE IS NO DATA?
if (get(xbee, 'BytesAvailable')==0)
    disp('Data not avail yet.   Try again or check transmitter.')
    return
end
%% Initializinig the rolling plot

buf_len = 100;
index = 1:buf_len;

% create variables for the Xaxis
gxdata = zeros(buf_len,1);
gydata = zeros(buf_len,1);
gzdata = zeros(buf_len,1);
axdata = zeros(buf_len,1);
aydata = zeros(buf_len,1);
azdata = zeros(buf_len,1);
Tdata = zeros(buf_len,1);
Pdata = zeros(buf_len,1);
Ydata = zeros(buf_len,1);

% %% Figure options
% figure(1);
% title('theta');
% %hold on;
% grid on;
% ax = axes('box','on');
% 
%             % Figure options
%             figure(2);
%             title('w');
%             hold on;
%             grid on;
%             ax = axes('box','on');
%             % Figure options
%             figure(3);
%             title('acc');
%             hold on;
%             grid on;
%             ax = axes('box','on');

wx = 0;
gxFilt = 0;
gyFilt = 0;
gzFilt = 0;
TFilt = 0;
PFilt = 0;
YFilt = 0;
alpha = 0.4;
disp('Filtering amplitude [0;1]: ');
disp(alpha);
%% IF THERE IS DATA
while (read & abs(wx) < 1000)% abs(wy) < 1000)
    %% Polling 
    fprintf(xbee, 'M') ; 
    try
        while (get(xbee, 'BytesAvailable')~=0)
            % read until terminator
            sentence = fscanf( xbee, '%s'); % this reads in as a string (until a terminater is reached)
            if (strcmp(sentence(1,1),'R'))
            %decodes "sentence" seperated (delimted) by commaseck Unit')
            [Roll, theta, Pitch, pitch, Yaw, yaw, OmegaX, wx, OmegaY, wy, OmegaZ, wz, AccX, ax, AccY, ay, AccZ, az] = strread(sentence,'%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f',1,'delimiter',',');

            %% Plotting angles
            figure(1);
            grid on;
            %disp('F1');
            % Update the rolling plot. Append the new reading to the end of the
            % rolling plot data
            if (theta>90)
                theta = theta - 360;
            end
            if (pitch > 90)
                pitch =  pitch - 360;
            end 
            
            % Apply noise filtering
            TFilt = (1 - alpha)*TFilt + alpha*theta;
            PFilt = (1 - alpha)*PFilt + alpha*pitch;
            YFilt = (1 - alpha)*YFilt + alpha*yaw;
            
            Tdata = [ Tdata(2:end) ; TFilt ];
            Pdata = [ Pdata(2:end) ; PFilt ];
            Ydata = [ Ydata(2:end) ; YFilt ];    
            %Plot the X magnitude
            subplot(3,1,1);
            grid on;
            title('Roll in deg');
            plot(index,Tdata,'r','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            xlabel('Time');
            ylabel('Theta (deg)');
            axis([1 buf_len -5 5]);
            %hold on;
            subplot(3,1,2);
            grid on;
            title('pitch in deg');
            plot(index,Pdata,'b','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            xlabel('Time');
            ylabel('phi in deg');
            axis([1 buf_len -5 5]);
            subplot(3,1,3);
            grid on;
            title('Yaw in deg');
            %hold on;
            plot(index,Ydata,'g','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            axis([1 buf_len 0 365]);
            xlabel('Time');
            ylabel('Psi in deg');
            
            %% Plotting angular velocities
            % Figure options
            figure(2);
            % Update the rolling plot. Append the new reading to the end of the
            % rolling plot data
            
            gxFilt = (1 - alpha)*gxFilt + alpha*wx;
            gyFilt = (1 - alpha)*gyFilt + alpha*wy;
            gzFilt = (1 - alpha)*gzFilt + alpha*wz;
            
            gxdata = [ gxdata(2:end) ; wx ];
            gydata = [ gydata(2:end) ; wy ];
            gzdata = [ gzdata(2:end) ; wz ];    
            %Plot the X magnitude
            subplot(3,1,1);
            title('X angular velocity in deg/s');
            plot(index,gxdata,'r','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            xlabel('Time');
            ylabel('Wx');
            axis([1 buf_len -20 20]);
            %hold on;
            subplot(3,1,2);
            title('Y angular velocity in deg/s');
            plot(index,gydata,'b','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            xlabel('Time');
            ylabel('Wy Acc');
            axis([1 buf_len -20 20]);
            subplot(3,1,3);
            title('Z angular velocity in deg/s');
            %hold on;
            plot(index,gzdata,'g','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            axis([1 buf_len -50 50]);
            xlabel('Time');
            ylabel('Wz Acc');
            
            %% Plotting accelerations
            % Figure options
            figure(3);
            % Update the rolling plot. Append the new reading to the end of the
            % rolling plot data
            axdata = [ axdata(2:end) ; ax ];
            aydata = [ aydata(2:end) ; ay ];
            azdata = [ azdata(2:end) ; az ];    
            %Plot the X magnitude
            subplot(3,1,1);
            title('X Axis Acceleration in G');
            plot(index,axdata,'r','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            xlabel('Time');
            ylabel('X Acc (G)');
            axis([1 buf_len -2.3 2.3]);
            %hold on;
            subplot(3,1,2);
            title('Y Axis Acceleration in G');
            plot(index,aydata,'b','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            xlabel('Time');
            ylabel('Y Acc');
            axis([1 buf_len -2.3 2.3]);
            subplot(3,1,3);
            title('Z Axis Acceleration in G');
            %hold on;
            plot(index,azdata,'g','LineWidth',1);%,'MarkerEdgeColor','k','MarkerFaceColor','g','MarkerSize',5);
            axis([1 buf_len -2.3 2.3]);
            xlabel('Time');
            ylabel('Z Acc');
            
            drawnow;
            end
        end
   end
end
fclose(xbee);
delete(xbee);
clear xbee