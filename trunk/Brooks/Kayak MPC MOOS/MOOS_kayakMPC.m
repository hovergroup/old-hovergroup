% Model Predictive Control for cross-track error
% Hovergroup kayaks, using MOOS, iMatlab
% Solved with CVX

% BR, 8/13/2012

% changes
%{
- 8/14/2012: added set point stuff (cut out heading error wrap for now)
- 8/14/2012: some minor changes while testing
- 8/17/2012: added increment to N, adjusted xDes increments (still a bit
off though).  

%}
clear all
close all
clc
format compact

% configure MPC parameters
configureKayakMPC;

% init
xDes = zeros(n,T+2);xDes(3,:)=desBearing(1:T+2);
eDes = zeros(n,T+2);
uPlan = zeros(m,T);

% configure MOOS/iMatlab parameters

moosDB = 'terra.moos';
pathName = '/home/josh/hovergroup/ivp-extend/brooks/missions/';
% pathName = '/home/mei/hovergroup/ivp-extend/mei/missions/mei_relay/';
old = cd(pathName);
% subscribe to
% 'MPC_XEST'% 'MPC_STOP'% 'GPS_X'% 'GPS_Y'% 'COMPASS_HEADING_FILTERED'
iMatlab('init','CONFIG_FILE',moosDB);

% reset MPC_STOP, wait a bit, and clear mail buffer
iMatlab('MOOS_MAIL_TX','MPC_STOP','STOP')
pause(1)
garbageMail = iMatlab('MOOS_MAIL_RX');

send = 0+rOff;
iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',send);
fprintf('Sending rudder = %f \n',send);

% wait for MOOS side to start, and get initial X, Y, H
stateReadTimeout = 2;   % time to wait for reading state...
mpc_stop=1;
gotStartPos=0;
gotX0=0;
gotY0=0;
gotH0=0;
while((mpc_stop || ~gotStartPos))   
    disp('Waiting for MOOS')
    mail = iMatlab('MOOS_MAIL_RX');
    messages = length(mail);
    for m=1:messages
        key = mail(m).KEY;
        if(strcmp(key,'MPC_STOP'))
            stopstr = mail(m).STR;
            if(strcmp(mail(m).STR,'GO'))
                mpc_stop=0;
                fprintf('Received MPC GO\n')
            end
        elseif(strcmp(key,'GPS_X'))
            x0 = mail(m).DBL;
            gotX0 = 1;
        elseif(strcmp(key,'GPS_Y'))
            y0 = mail(m).DBL;
            gotY0 = 1;
        elseif(strcmp(key,'COMPASS_HEADING_FILTERED'))
            h0 = mail(m).DBL+trueNorthAdjustment;
            gotH0 = 1;
        end
        gotStartPos=min([gotX0,gotY0,gotH0]);
        if(gotStartPos);fprintf('Got Start Position');end
    end
    pause(0.5)
end


% initial estimate of ERROR is all zero...(first trackline defined off
% initial measurement)
eEst = [0;0;0;0];
mpcStart = tic;

% initial start position
hDes = atan2((y(1)-y0),(x(1)-x0));
hDes = rad2deg(hDes);
bearing = 90 - hDes;
if(bearing<0)
    bearing = bearing+360;
end
if(bearing>360)
    bearing = bearing - 360;
end
%eh0 = bearing-h0;

% start distance for 1st waypoint
distStart = sqrt((y0-y(1))^2+(x0-x(1))^2);
leg1Time = distStart/speed;
leg1Steps = floor(leg1Time/dt);
fprintf('\n Leg 1 @ %f bearing, %f sec, %i steps\n',bearing,leg1Time,leg1Steps)

N = N+leg1Steps;
desBearing = [bearing*ones(1,leg1Steps) desBearing];

loopIt=1;
% start loop  (breaks when MPC_STOP==1)
while(~mpc_stop)
    loopStart = tic;
    step = floor(toc(mpcStart)/dt)+1;
    fprintf('Step: %i, loopIt: %i\n\n',step,loopIt)
    
    if(loopIt>1)
        % note - xEst is ERRORS
        % cross-track and heading error relative to desBearing(step)
        [eEst mpc_stop] = parseMPC_XEST;
    end
    xHat = eEst + [0 0 desBearing(loopIt) 0]';
    fprintf('State estimate\n')
    fprintf('%s %f\n','ehddot', eEst(1), 'ehdot ', ...
        eEst(2), 'eh    ', eEst(3), 'ex    ', eEst(4))
    fprintf('actual est heading: %f [deg]\n',xHat(3))
    fprintf('cross-track error: %f [m]\n\n',Cd(2,4)*eEst(4))
    
    % compute xDes in MPC coord frame
    % first construct xDes vector in bearing coord frame
    xDes = zeros(n,T+2);
    eDes = zeros(n,T+2);
    if(loopIt==1)
        xDes(3,:) = desBearing(1:(loopIt+T+1));
    elseif((loopIt+T+1)<N)
        xDes(3,:) = desBearing((loopIt-1):(loopIt+T));
    else
        pad = N-loopIt;
        xDes(3,:) = [desBearing(loopIt:loopIt+pad-1) desBearing(N-1)*ones(1,T+2-pad)];
    end
    fprintf('Step %i, desired bearing = %f \n\n',step,desBearing(step))
    
    % MPC coord frame is defined relative to desBearing(i)
    eDes(3,:) = xDes(3,:) - ones(size(xDes(3,:)))*desBearing(step);
    
    % wrap xDes to +/- 180 deg
    for j = 1:(T+2)
        if(eDes(3,j) > 180)
            eDes(3,j) = eDes(3,j) - 360;
        end
        if(xDes(3,j) < (-180))
            eDes(3,j) = eDes(3,j) + 360;
        end
    end
    disp(eDes(3,:))
    
    uPrev = uPlan(:,1);    
    % solve MPC - xEst and previous control are inputs
    [uPlan tMPC] = solveKayakMPC(sys,eEst,MPCparams,uDelay,uPrev,eDes);
    fprintf('Computed plan: \n')
    disp(uPlan)
    
    % outputs plan of desired rudder
    
    % (encode, quantize plan)
    
    while(toc(loopStart)<(dt-.005))
        pause(0.005)
    end
    
    % send plan (just control to start)
    
    send = uPlan(:,1)+rOff;
    
    iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',send);
    fprintf('\n\nSending rudder = %f \n\n\n',send);
    
    % string to log
    MPC_STR = 'uPlan';
    for k = 1:T
        MPC_STR = [MPC_STR ',' num2str(uPlan(k))];
    end
    MPC_STR = [MPC_STR sprintf(',tMPC,%0.3f',tMPC)];
    
    % send a big string of a bunch of other stuff:
    iMatlab('MOOS_MAIL_TX','MPC_STR',MPC_STR)
    
    % check if at end
    if(loopIt>=(N-1+leg1Steps))
        mpc_stop=1;
        disp('FINISHED TRACKLINE LIST')
    end
    loopIt = loopIt+1;
    
    loopTime = toc(loopStart)
    
end

cd(old)


