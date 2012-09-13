

close all

format compact

global dt

%% ICs

p=0.8; %probability of packet success

%% Loop
%configureKayakMPC;
initializeMOOS_MPC;
mpcStart = tic;
loopIt=1

% uPlanSave = cell(1,N+1);
% XSave = cell(1,N+1);
% eEstSave = zeros(n,N+1);

diary on
% start loop  (breaks when MPC_STOP==1)
%packetsuccess=zeros(1,1000);
%usent=zeros(1,1000);
clear packetsuccess
clear usent
clear GPSxsave
clear GPSysave
clear headingsave
clear statesave1
clear statesave2
clear z1save
clear z2save

while(loopIt<1000)
    
    loopStart = tic;
    ucontrol=0;
    if(loopIt>1)
        clear ucontrol
        % cross-track and heading ERROR relative to desBearing(step)
        %[eEstKF mpc_stop] = parseMPC_XEST(n,sys);
        %%

        [heading GPSx GPSy] = parseMPC_XEST %convert moos gps, heading variables to matlab variables
        [Ginfo1out Ginfo2out ] = GuptaEncode(heading, GPSx, GPSy) %change measurements to gupta numbers
            % DELAY (simulate acomms delay dt between sensor and controller...)
            while(toc(loopStart)<(dt-.005))
                pause(0.005)
            end
        [ucontrol packet state] = GuptaController(Ginfo1out, Ginfo2out, p) %generate control command, with sensor packet success probability p
packetsuccess(loopIt)=packet;
usent(loopIt)=ucontrol;
headingsave(loopIt)=heading;
GPSxsave(loopIt)=GPSx;
GPSysave(loopIt)=GPSy;
statesave1(loopIt)=state(1);
statesave2(loopIt)=state(2);

%%%%%%%%%%%%
x1 = 20;
y1 = 0;
x2 = 20;
y2 = -800;
h1 = 180;

a = (y1-y2);
b = (x2-x1);
c = (-y1*(x2-x1)+x1*(y2-y1));

z1 = (heading  - h1)*pi/180;
z2 = -(a*GPSx+b*GPSy+c)/sqrt(a^2+b^2);

z2save(loopIt)=z1;
z1save(loopIt)=z2;

%%%%%%%%%%
%xhatsave(loopIt)=xhatGupta;
close all
figure
subplot(2,1,1);
plot(statesave1,'--r');
hold on
plot(z1save,'b');

subplot(2,1,2);
plot(statesave2,'--r');
hold on
plot(2*z2save);


          % usend = str2num('ucontrol')
    switch TX
        case 'wifi'
            send = sprintf('heading = %0.1f',ucontrol);
            iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',ucontrol); %post control command to MOOS
            fprintf('\n\nSend des heading = %f \n\n\n',ucontrol);
            
    end
        %ucontrol = InfoController(heading, GPSx, GPSy, p); %generate
        %control command using info filter, with sensor packet success
        %probability p
        
    end
    
    
    % (encode, quantize plan)
    % send plan (just control to start)
%     usend = str2num('ucontrol');
    
%     switch TX
%         case 'wifi'
%             send = sprintf('heading = %0.1f',ucontrol);
%             iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',usend); %post control command to MOOS
%             fprintf('\n\nSend des heading = %f \n\n\n',ucontrol);
%             
%     end
    
    loopIt = loopIt+1
    
    loopTime = toc(loopStart);
    fprintf('\nLoop Time: %f sec\n',loopTime)
    
end

diary off

cd(old)

