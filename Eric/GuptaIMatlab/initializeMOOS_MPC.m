% function  [N desBearing eEst old] = initializeMOOS_MPC(rOff,N,desBearing)

% initialize MOOS MPC
% sets up iMatlab, initializes state estimate, calculates first trackline


%% Gupta variables to be initialized
%u, YC, YsaveC, Psi need to be initialized before algorithm run

global u YC YsaveC Psi uold xhatGupta
global Y Ysave Ginfo1 Ginfo2 dt yhat xhatInfo

u=0;
YC=eye(4,4);
YsaveC = YC;
Psi = zeros(4,1);
uold = u;
xhatGupta = zeros(4,1);
Y=YC;
Ysave = YsaveC;
Ginfo1 = zeros(4,1);
Ginfo2 = zeros(4,1);
yhat = zeros(4,1);
xhatInfo = zeros(4,1);

dt=6; %delay and time step
%%
TX = 'wifi';
%TX = 'acomms';

% configure MOOS/iMatlab parameters

moosDB = 'terra.moos';
pathName1 = '/home/josh/hovergroup/ivp-extend/brooks/missions/';
pathName2 = '/home/eric/hovergroup/ivp-extend/eric/missions/';
% pathName = '/home/mei/hovergroup/ivp-extend/mei/missions/mei_relay/';

try 
    old = cd(pathName1);
end
try
   old = cd(pathName2); 
end


% subscribe to (subscribe vars in .moos process config)
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
while(( ~gotStartPos))   
    disp('Waiting for MOOS')
    mail = iMatlab('MOOS_MAIL_RX');
    messages = length(mail);
    for m=1:messages
        key = mail(m).KEY;
        if(strcmp(key,'GPS_X'))
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

