% t = linspace(0,100)
% 
% pwm = 650*sin(exp(t))+700
% plot(t,pwm)
% 
% %%
% 
% t = 0:1:100
% 
% pwm = sin(exp(t))
% plot(t,pwm)
% 
% %%
%  Fs=1000; % sample rate 
% tf=2; % 2 seconds
% t=0:1/Fs:tf-1/Fs;
% f1=100;
% f1=f1*t %+(sl.*t/2);
% %f2=f1(end)+f2*semi_t-sl.*semi_t/2;
% %f=[f1 f2];
% f = f1
% y=650*cos(2*pi*f.*t)+ 1300;
% plot(t,y)


%%

%% Arduino RPM real time plot



    clear all;
    clc;
    delete('instrfindall');
    
    finished = false;
    recording = true;
    if recording 
       disp('Record data enabled');
    end
    
%% Check serial objects & Connect to Port

%   Check the port you are using with the arduino, then run: sudo ln -s /dev/ttyNUMBER /dev/ttyS101
    disp('Linux User? Have you run the simbolic link with ttyS101?');
    disp('sudo rm /dev/ttyS101');
    disp('sudo ln -s /dev/tty_NUMBER_ /dev/ttyS101');
    port = '/dev/ttyS101';
    oldSerial = instrfind('Port', port); 
    % can also use instrfind() with no input arguments to find ALL existing serial objects
    if (~isempty(oldSerial))  % if the set of such objects is not(~) empty
        disp('WARNING:  Port in use. Check your port. Closing.')
        delete(oldSerial)
        a = true;
    else        
        disp('Connection to serial port...');
    end
    
%%  Setting up serial communication

    % XBee expects the end of commands to be delineated by a carriage return.
    xbee = serial(port,'baudrate',9600,'terminator','CR','tag','Quad');

    set(xbee, 'TimeOut', 15);  %I am willing to wait 1.5 secs for data to arive
    % I wanted to make my buffer only big enough to store one message
    set(xbee, 'InputBufferSize',  390 )
    % Before you can write to your serial port, you need to open it:
    fopen(xbee);
    
    disp('Serial port opened');
    
%% Disp incoming serial message

    fprintf(xbee,'T')
    disp(fscanf(xbee,'%s'));
    
%% Testing Connection 
    
    % Ask for data
    fprintf(xbee,'T');
    disp('Sent: T');
    pause(1);
    ack = fscanf( xbee, '%s');
    if (strcmp(deblank(ack), 'Ok') == 1)
        disp ('Received: OK');
    else
        disp ('Communication problem. Check Hardware and retry.');
    end       
    
    % Take Off
    pause(1);
    fprintf(xbee,'c');
    disp('Sent: calibrate command');
    pause(5);
    ack = fscanf( xbee, '%s')
    if (strcmp(deblank(ack), '...') == 1)
        disp ('takiing off');
    else
        disp ('Communication problem. Check Hardware and retry.');
    end     
    
    disp('Sent:  i');
    fprintf(xbee,'i');
    pause(4)
    ack = fscanf( xbee, '%s')
    if (strcmp(deblank(ack), 'Initialized') == 1)
        disp (' OK Initialized');
    else
        disp ('Communication problem. Check Hardware and retry.');
    end  
    
    % Land
    
% 
%     fprintf(xbee,'l');
%     disp('Sent: l');
%     pause(1);
%     ack = fscanf( xbee, '%s')
%     if (strcmp(deblank(ack), 'Landing') == 1)
%         disp ('Landing');
%     else
%         disp ('Communication problem. Check Hardware and retry.');
%     end     
%     pause(3)
%     ack = fscanf( xbee, '%s')
%     if (strcmp(deblank(ack), 'Landed') == 1)
%         disp ('Landed');
%     else
%         disp ('Communication problem. Check Hardware and retry.');
%     end       
    
    
%% Ask desired Sample Rate in Hz
    rate=input('Enter the samplerate in Hz. Max 500Hz. Non sgravare...      ');
    delay = 1/rate;
    str = sprintf('SampleRate fixed to: %f.', delay);
    disp(str);

%% Sending commands


    fprintf(xbee,'t');
    disp('Sent: t');
    pause(1);  
    
%% Initializing

buf_len = rate*1000;
    index = 1:buf_len;

    % create variables for the Xaxis
    time = zeros(buf_len,1);
    output = zeros(buf_len,1);
    input = zeros(buf_len,1);

    if recording
        %response = input('Press r to record 2 seconds of data');
        %if (strcmp(response,'r'))
            disp('Record!');
            record = true;
            numberOfSamples = 0;
        %end
    end
%% Data collection and Plotting
    while (~finished)
        % Polling 
        pause(delay);
        fprintf(xbee,'M') ; 
        notArrived = false;
        try
            while (get(xbee, 'BytesAvailable')~=0 && ~notArrived)
                % read until terminator
                sentence = fscanf( xbee, '%s'); % this reads in as a string (until a terminater is reached)
                if (strcmp(sentence(1,1),'A'))
                    notArrived = true;
                    %decodes "sentence" seperated (delimted) by commaseck Unit')
                    C = textscan(sentence,'%c %d %d %d %c','delimiter',',');
                    Wx = C{2}
                    Wy = C{3}
                    Wz = C{4}

                    % [gx, gy, gz] = [x, y, z];
                    time = [ time(2:end) ; double(Wx) ];
                    output = [ output(2:end) ; double(Wy) ];
                    input = [ input(2:end) ; double(Wz) ]; 

                    numberOfSamples = numberOfSamples + 1

                    % wait one second then record
                    if numberOfSamples==rate*30 && recording
                        disp('saving samples to file');
                        gDataToWrite = [time output input];
                        csvwrite('samples.txt',gDataToWrite);
                        disp('saving file to structure');
                        dat.x = time;
                        dat.y = output;
                        dat.z = input;
                        save('samples.mat','-struct','dat');
                        disp('Saved.');

                        finished = true;
                    end
                end
            end
        catch exeption
            disp('Warning');
        end
    end
    if finished
       disp('Check your local folder.');
    end
%% Close connection    


    fprintf(xbee,'l');
    disp('Sent: l');
    pause(1);
    ack = fscanf( xbee, '%s')
    if (strcmp(deblank(ack), 'Landing') == 1)
        disp (' OK Landing');
    else
        disp ('Communication problem. Check Hardware and retry.');
    end     
    pause(3)
    ack = fscanf( xbee, '%s')
    if (strcmp(deblank(ack), 'Landed') == 1)
        disp (' OK Initialized');
    else
        disp ('Communication problem. Check Hardware and retry.');
    end       
    
    fclose(xbee);
    delete(xbee);
    clear xbee
clear('instrfindall');