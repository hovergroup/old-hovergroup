
clear all
close all
clc
format compact

global dt

%% ICs
r0=0;       % if cross-track... initial rudder
hd0=200;    % if heading setpt... initial heading
% hd0 SHOULD MATCH STARTING CONSTANT HEADING BEHAVIOR SETPT

%% Loop
%configureKayakMPC;
initializeMOOS_MPC;
mpcStart = tic;
loopIt=1;

uPlanSave = cell(1,N+1);
XSave = cell(1,N+1);
eEstSave = zeros(n,N+1);

diary on
% start loop  (breaks when MPC_STOP==1)

while(loopIt<1000)
    
    loopStart = tic;
    
    if(loopIt>1)
        % cross-track and heading ERROR relative to desBearing(step)
        %[eEstKF mpc_stop] = parseMPC_XEST(n,sys);
        %%
        [heading GPSx GPSy] = parseMPC_XEST; %convert moos gps, heading variables to matlab variables
        [Ginfo1out Ginfo2out ] = GuptaEncode(heading, GPSx, GPSy); %change measurements to gupta numbers
            % DELAY (simulate acomms delay dt between sensor and controller...)
            while(toc(loopStart)<(dt-.005))
                pause(0.005)
            end
        ucontrol = GuptaController(Ginfo1control, Ginfo2control, p); %generate control command, with sensor packet success probability p
        %ucontrol = InfoController(heading, GPSx, GPSy, p); %generate
        %control command using info filter, with sensor packet success
        %probability p
        
    end
    
    
    % (encode, quantize plan)
    % send plan (just control to start)
    
    
    switch TX
        case 'wifi'
            send = sprintf('heading = %0.1f',ucontrol);
            iMatlab('MOOS_MAIL_TX','CONST_HEADING_UPDATES',send); %post control command to MOOS
            fprintf('\n\nSend des heading = %f \n\n\n',ucontrol);
            
            
        case 'acomms'
            
            %
            
    end
    
    loopIt = loopIt+1;
    
    loopTime = toc(loopStart);
    fprintf('\nLoop Time: %f sec\n',loopTime)
    
end

diary off

cd(old)

