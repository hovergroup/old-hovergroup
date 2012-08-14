% Model Predictive Control for cross-track error
% Hovergroup kayaks, using MOOS, iMatlab
% Solved with CVX

% BR, 8/13/2012

% changes
%{
- 8/14/2012: added set point stuff (cut out heading error wrap for now)
%}
clear all
close all
clc
format compact

% configure MPC parameters
configureKayakMPC;

% init
xDes = zeros(n,T+2);xDes(3,:)=desBearing(1:T+2);
uPlan = zeros(1,T);

% configure MOOS/iMatlab parameters

moosDB = 'terra.moos';
pathName = '/home/josh/hovergroup/ivp-extend/brooks/missions/';
old = cd(pathName);
iMatlab('init','CONFIG_FILE',moosDB);
% note -
% subscribe to
% 'MPC_XEST'
% 'MPC_STOP'
% 'GPS_X'
% 'GPS_Y'
% 'COMPASS_HEADING_FILTERED'
%
% (others?)


stateReadTimeout = 2;   % time to wait for reading state...
iMatlab('MOOS_MAIL_TX','MPC_STOP','STOP')
pause(1)
garbageMail = iMatlab('MOOS_MAIL_RX');

% wait for MOOS side to start, and get initial X, Y, H
MPC_STOP=1;
gotStartPos=0;
gotX0=0;
gotY0=0;
gotH0=0;
while((MPC_STOP || ~gotStartPos))
    mail = iMatlab('MOOS_MAIL_RX');
    messages = length(mail);
    for m=1:messages
        key = mail(m).KEY
        if(strcmp(key,'MPC_STOP'))
            stopstr = mail(m).STR
            if(strcmp(mail(m).STR,'GO'))
                MPC_STOP=0;
            end
        elseif(strcmp(key,'GPS_X'))
            x0 = mail(m).DBL;
            gotX0 = 1;
        elseif(strcmp(key,'GPS_Y'))
            y0 = mail(m).DBL;
            gotY0 = 1;
        elseif(strcmp(key,'COMPASS_HEADING_FILTERED'))
            h0 = mail(m).DBL;
            gotH0 = 1;
        end
        gotStartPos=min([gotX0,gotY0,gotH0])
        
    end
    disp('Waiting for MOOS')
    pause(1)
end
mpcStart = tic;
disp('Start - got initial state')

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
xEst = [0;0;h0;0];

% start distance for 1st waypoint
distStart = sqrt((y0-y(1))^2+(x0-x(1))^2);
leg1Time = distStart/speed;
leg1Steps = ceil(leg1Time/dt);
fprintf('\n Leg 1 @ %f bearing, %i steps\n',bearing,leg1Steps)

desBearing = [bearing*ones(1,leg1Steps) desBearing];

loopIt=1;
%xEst=zeros(4,1);
% start loop  (breaks when MPC_STOP==1)
while(~MPC_STOP)
    loopStart = tic;
    if(loopIt>1)
        % read in KF estimate
        gotState=0;
        stateReadStart = tic;
        while(~gotState)
            mail=iMatlab('MOOS_MAIL_RX');
            messages=length(mail);
            
            if messages==0
                continue;
            end
            
            % Process messages
            
            % string of states:
            % 'MPC_XEST' = 'x1<|>x2<|>x3<|>x4<|>'
            
            key = cell(1,messages);
            val = cell(1,messages);
            str = cell(1,messages);
            m = 1;
            while(~gotState)
                
                %msg_tic = msg_tic+1
                
                key{m}=mail(m).KEY;
                val{m}=mail(m).DBL;
                str{m}=mail(m).STR;
                
                switch key{m}
                    
                    case 'MPC_XEST'
                        
                        xE = cell(1,4);
                        xEst = zeros(4,1);
                        % parse state string
                        stateString = str{m};
                        try
                            xE = textscan(stateString,...
                                '%f %*s %*s %f %*s %*s %f %*s %*s %f' ,...
                                'Delimiter','<|>');
                            xEst = [xE{1};xE{2};xE{3};xE{4}];
                            fprintf('hddot: %f hdot: %f h: %f ex [m]: %f \n',...
                                xEst(1),xEst(2),xEst(3),Cd(2,4)*xEst(4))
                            gotState = 1;
                        catch
                            disp('error parsing state string')
                        end
                        
                    case 'MPC_STOP'
                        
                        MPC_STOP=val{m};
                        
                end
                
                % if more messages to look through:
                if(m<messages)
                    m = m+1;
                else
                    continue
                end
                
            end
            
            % check timeout on reading state
            if(toc(stateReadStart)>stateReadTimeout)
                disp('TIMEOUT READING IN STATES - USING OLD STATE')
                continue
            end
            
            
            
        end
    end
    
    % (desired trackline for this time)
    step = floor(toc(mpcStart)/dt)+1;
    desiredHeading = desBearing(step);
    
    
    xDes = zeros(n,T+2);
    if((loopIt+T+1)<N)
        xDes(3,:) = desBearing(loopIt:(loopIt+T+1));
    else
        pad = N-loopIt;
        xDes(3,:) = [desBearing(loopIt:loopIt+pad-1) desBearing(N-1)*ones(1,T+2-pad)];
    end
    setPt=1;
    
    %     % map heading from [0,360] to [-180,+180]
    %     errorHeading = desiredHeading - xEst(3);
    %     if(errorHeading>180)
    %         errorHeading = errorHeading - 360;
    %     end
    %     if(errorHeading<-180)
    %         errorHeading = errorHeading + 360;
    %     end
    
    % replace state 3 with heading error
    %    xEst(3) = errorHeading;
    
    xEst
    xDes
    uPrev = uPlan(:,1)
    % solve MPC - xEst and previous control are inputs
    [uPlan tMPC] = solveKayakMPC(sys,xEst,MPCparams,uDelay,uPrev,setPt,xDes);
    uPlan
    
    % outputs plan of desired rudder
    
    % (encode, quantize plan)
    
    while(toc(loopStart)<(dt-0.2))
        pause(0.01)
    end
    
    
    % send plan (just control to start)
    send = uPlan(:,1);
    iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',send);
    fprintf('Sending rudder = %f \n',send);
    
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
        MPC_STOP=1;
        disp('FINISHED TRACKLINE LIST')
    end
    loopIt = loopIt+1;
    
    loopTime = toc(loopStart)
    
end


cd(old)



%{

*********** iMatlab Initialising ***********
* A box of MOOS accessories                *
* P. Newman                                *
* Oxford 2005                              *
********************************************
Read String CONFIG_FILE
Property CONFIG_FILE                Str: terra.moos
Property MOOSNAME                   Str: iMatlab
Property SERIAL                     Dbl: 0.000
Property SERIAL_TIMEOUT             Dbl: 10.000
Property SERVERHOST                 Str: localhost
Property SERVERPORT                 Dbl: 9000.000
Setting Up MOOS Comms
Adding Registration for "COMPASS_HEADING_FILTERED" every 1.000000 seconds 
Adding Registration for "GPS_Y" every 1.000000 seconds 
Adding Registration for "GPS_X" every 1.000000 seconds 
Adding Registration for "MPC_STOP" every 0.000000 seconds 
Adding Registration for "MPC_XEST" every 0.000000 seconds 
Adding Registration for "DB_TIME" every 1.000000 seconds 
DB connection established.
Waiting for MOOS
key =
DB_TIME
gotStartPos =
     0
key =
COMPASS_HEADING_FILTERED
gotStartPos =
     0
key =
GPS_Y
gotStartPos =
     0
key =
GPS_X
gotStartPos =
     1
Waiting for MOOS
key =
DB_TIME
gotStartPos =
     1
key =
COMPASS_HEADING_FILTERED
gotStartPos =
     1
key =
GPS_Y
gotStartPos =
     1
key =
GPS_X
gotStartPos =
     1
Waiting for MOOS
key =
DB_TIME
gotStartPos =
     1
key =
COMPASS_HEADING_FILTERED
gotStartPos =
     1
key =
GPS_Y
gotStartPos =
     1
key =
GPS_X
gotStartPos =
     1
Waiting for MOOS
key =
DB_TIME
gotStartPos =
     1
key =
COMPASS_HEADING_FILTERED
gotStartPos =
     1
key =
GPS_Y
gotStartPos =
     1
key =
GPS_X
gotStartPos =
     1
Waiting for MOOS
key =
DB_TIME
gotStartPos =
     1
key =
COMPASS_HEADING_FILTERED
gotStartPos =
     1
key =
GPS_Y
gotStartPos =
     1
key =
GPS_X
gotStartPos =
     1
Waiting for MOOS
key =
DB_TIME
gotStartPos =
     1
key =
COMPASS_HEADING_FILTERED
gotStartPos =
     1
key =
GPS_Y
gotStartPos =
     1
key =
GPS_X
gotStartPos =
     1
Waiting for MOOS
key =
MPC_STOP
stopstr =
GO
gotStartPos =
     1
key =
DB_TIME
gotStartPos =
     1
key =
MPC_XEST
gotStartPos =
     1
key =
COMPASS_HEADING_FILTERED
gotStartPos =
     1
key =
GPS_Y
gotStartPos =
     1
key =
GPS_X
gotStartPos =
     1
key =
MPC_XEST
gotStartPos =
     1
Waiting for MOOS
Start - got initial state

 Leg 1 @ 78.517561 bearing, 4 steps
xEst =
         0
         0
   33.4640
         0
xDes =
  Columns 1 through 10
         0         0         0         0         0         0         0         0         0         0
         0         0         0         0         0         0         0         0         0         0
   78.5176   78.5176   78.5176   78.5176   73.0000   73.0000   73.0000   73.0000   73.0000   73.0000
         0         0         0         0         0         0         0         0         0         0
  Columns 11 through 12
         0         0
         0         0
   73.0000   73.0000
         0         0
uPrev =
     0
 
Calling sedumi: 271 variables, 68 equality constraints
   For improved efficiency, sedumi is solving the dual problem.
------------------------------------------------------------
SeDuMi 1.21 by AdvOL, 2005-2008 and Jos F. Sturm, 1998-2003.
Alg = 2: xz-corrector, Adaptive Step-Differentiation, theta = 0.250, beta = 0.500
Put 36 free variables in a quadratic cone
eqs m = 68, order n = 191, dim = 273, blocks = 4
nnz(A) = 551 + 0, nnz(ADA) = 2748, nnz(L) = 1408
 it :     b*y       gap    delta  rate   t/tP*  t/tD*   feas cg cg  prec
  0 :            1.42E+05 0.000
  1 :   3.51E+04 6.03E+04 0.000 0.4262 0.9000 0.9000   4.81  1  1  3.7E+00
  2 :   1.26E+04 2.18E+04 0.000 0.3619 0.9000 0.9000   2.15  1  1  8.9E-01
  3 :   2.38E+03 1.04E+04 0.000 0.4773 0.9000 0.9000   3.06  1  1  2.6E-01
  4 :  -3.18E+01 3.02E+03 0.000 0.2895 0.9000 0.9000   1.78  1  1  1.1E-01
  5 :  -4.26E+02 1.29E+03 0.000 0.4290 0.9000 0.9000   1.26  1  1  1.3E-01
  6 :  -5.73E+02 6.92E+02 0.000 0.5346 0.9000 0.9000   1.06  1  1  7.7E-02
  7 :  -6.02E+02 5.34E+02 0.000 0.7714 0.9000 0.9000   0.66  1  1  7.8E-02
  8 :  -7.18E+02 2.08E+02 0.000 0.3904 0.9000 0.9000  -0.89  1  1  2.4E-01
  9 :  -9.80E+02 9.84E+01 0.000 0.4722 0.9000 0.9000  -0.82  1  1  3.3E-01
 10 :  -1.97E+03 2.74E+01 0.000 0.2783 0.9000 0.9000  -0.89  1  1  3.0E-01
 11 :  -8.10E+03 5.28E+00 0.000 0.1930 0.9000 0.9000  -0.99  1  1  8.0E-03
 12 :  -3.58E+04 1.16E+00 0.000 0.2202 0.9000 0.9000  -1.00  1  1  8.0E-03
 13 :  -6.91E+05 5.83E-02 0.000 0.0501 0.9900 0.9900  -1.00  1  1  7.7E-03

Dual infeasible, primal improving direction found.
iter seconds  |Ax|    [Ay]_+     |x|       |y|
 13      0.2   4.7e-05   5.0e+00   3.5e+00   5.7e+01

Detailed timing (sec)
   Pre          IPM          Post
7.000E-02    2.100E-01    6.000E-02    
Max-norms: ||b||=20, ||c|| = 5.729578e+03,
Cholesky |add|=0, |skip| = 0, ||L.L|| = 6.50464.
------------------------------------------------------------
Status: Infeasible
Optimal value (cvx_optval): +Inf
uPlan =
   NaN   NaN   NaN   NaN   NaN   NaN   NaN   NaN   NaN   NaN
Sending rudder = NaN 
loopTime =
    2.8190
hddot: 3.720770 hdot: -1.434820 h: 2.326880 ex [m]: 0.081391 
xEst =
    3.7208
   -1.4348
    2.3269
    2.3317
xDes =
  Columns 1 through 10
         0         0         0         0         0         0         0         0         0         0
         0         0         0         0         0         0         0         0         0         0
   78.5176   78.5176   78.5176   73.0000   73.0000   73.0000   73.0000   73.0000   73.0000   73.0000
         0         0         0         0         0         0         0         0         0         0
  Columns 11 through 12
         0         0
         0         0
   73.0000   73.0000
         0         0
uPrev =
   NaN
Error using cvx/plus (line 83)
Disciplined convex programming error:
   Illegal operation: {real affine} + {invalid}
Error in solveKayakMPC (line 79)
        X(:,2) == A*(X(:,1)) + Bin*xDes(:,1) + B*uPrev;
Error in MOOSkayakMPC (line 211)
    [uPlan tMPC] = solveKayakMPC(sys,xEst,MPCparams,uDelay,uPrev,setPt,xDes); 
clear iMatlab
iMatlab is cleaning up
Halting MOOSComms >> 





%}


