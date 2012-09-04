% function  [N desBearing eEst old] = initializeMOOS_MPC(rOff,N,desBearing)

% initialize MOOS MPC
% sets up iMatlab, initializes state estimate, calculates first trackline

% BR, 8/19/2012

% changes:
%{
-8/20/2012: added options for diff systems
- 8/29/2012: removed trueNorthAdjustment (compass does already)
- 9/3/2012 - initial xEst has very small nonzero values (avoids CVX
problem)
%}

% some preallocations and inits for script

% (n is number of MPC states)
% (KF has n states as well, but has ehddot instead of ePSI)
xDes = zeros(n,T+2);eDes = zeros(n,T+2);
% KF always has 5 states
eEstKF = zeros(5,1);
xHat = zeros(n,1);
uPlan = zeros(m,T);
kPlan = 1;  % no init packet loss
uPlanBuffered = uPlan;
u = 0;

% 5 + T*(lenU) + len_tMPC + len_ifPLoss
lenMPC_STR=5+T*10+19+14;
MPC_STR = char(97*ones(1,lenMPC_STR));


TX = 'wifi';
%TX = 'acomms';

% configure MOOS/iMatlab parameters

moosDB = 'terra.moos';
pathName1 = '/home/josh/hovergroup/ivp-extend/brooks/missions/';
pathName2 = '/home/brooks/hovergroup/ivp-extend/brooks/missions/';
% pathName = '/home/mei/hovergroup/ivp-extend/mei/missions/mei_relay/';

try 
    old = cd(pathName1);
end
try
   old = cd(pathName2); 
end


% subscribe to
% 'MPC_XEST'% 'MPC_STOP'% 'GPS_X'% 'GPS_Y'% 'COMPASS_HEADING_FILTERED'
iMatlab('init','CONFIG_FILE',moosDB);

% reset MPC_STOP, wait a bit, and clear mail buffer
iMatlab('MOOS_MAIL_TX','MPC_STOP','STOP')
pause(1)
garbageMail = iMatlab('MOOS_MAIL_RX');

send = hd0;
iMatlab('MOOS_MAIL_TX','DESIRED_HEADING',send);
fprintf('Sending des heading = %f \n',send);

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
            h0 = mail(m).DBL;   %+trueNorthAdjustment;(compass already)
            gotH0 = 1;
        end
        gotStartPos=min([gotX0,gotY0,gotH0]);
        if(gotStartPos);fprintf('Got Start Position');end
    end
    pause(0.5)
end

% initial estimate of ERROR is all zero...(first trackline defined off
% initial measurement)
switch n
    case 4
        eEst = [0;0;0;0];
    case 5
        eEst = [0;0;0;0;0];
end
eEst = eEst + 0.001;


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

% start distance for 1st waypoint
distStart = sqrt((y0-y(1))^2+(x0-x(1))^2);
leg1Time = distStart/speed;
leg1Steps = floor(leg1Time/dt);
fprintf('\n Leg 1 @ %f bearing, %f sec, %i steps\n',bearing,leg1Time,leg1Steps)

% increment length of mission in steps, add first trackline bearing
N = N+leg1Steps-1;
desBearing = [bearing*ones(1,leg1Steps-1) desBearing];
