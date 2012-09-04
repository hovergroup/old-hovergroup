% configureKayakMPC
% script for setting MPC and kayak model parameters
% used with kayak_MPC_KF AND MOOSkayakMPC
% calls generateTracklinesMPC

% changes
%{
- 8/14 changed system so that + rudder causes + heading (compass bearing),
added Bin
- 8/15/2012 - moved major settings here from generateTracklinesMPC
- 8/17/2012 - added kayak models
- 8/19/2012 - added packet loss params
- 8/19/2012 - added CL heading system
- 8/20/2012 - added option for CL heading to have n=3 or n=4
- 8/29/2012 - changed CL heading sys to n = 4 with psi as a state, and dPsi
    as control input (for sparse controls), modified ICs to reflect this
- 8/29/2012 - modified gen tracklines to work all on headings (not angles)
- 8/30/2012 - removed crossTrack with rudder input from this version
- 8/31/2012 - rearranged

        
%}

ifQuiet = 1;          % if cvx is run in quiet mode
uDelay = 1;

% packet loss probability:
probPLoss = .5;
%probPLoss = 0;


%% PARAMETERS

%syss = 'crossTrack';
syss = 'crossTrack_integrator';

speed = 1.8;    % nostromo modem
%speed = 1.2;
% speed = 0.8;  % kassandra modem


%%%
%hspeed = 1.5;
hspeed = 2;
%%%


% Planning horizon (steps)
%T = 15;
 T=32;
% T = 6;
% T = 15;

mu=1;              % sparse control weight

% Time step (sec)
%dt = 1;
%dt = 6;
%dt = 3;
dt = 6;

%tracklineType='straight';
tracklineType = 'oneturn';
%tracklineType = 'hexagon';

pavHeading = 73;
pavHeadingOffset = 0;
switch tracklineType
    case 'straight'
        %Nsec=30;
        Nsec=60;
        ox = -20;oy = -50;
        % bearing of straight line:
        %startHeading = 80;
        startHeading = pavHeading + pavHeadingOffset;
    case 'oneturn'
        secPerLeg = ceil(120/dt)*dt;
        %secPerLeg = ceil(60/dt)*dt;
        numLegs=2;
        Nsec = secPerLeg*numLegs;
        %ox = 0;oy = 150;      
        %ox = 230;
        %oy = 0;
        %startHeading = pavHeading + pavHeadingOffset;

        ox = -100;
        oy = -100;
        
        startHeading = 240;
        kinkAng = -30;
        %startHeading = 180;     
        %kinkAng = 30;  % second leg is after kink, + angle turns right
    case 'hexagon'
        numLegs = 6;
        secPerLeg = 90;
        Nsec = secPerLeg*numLegs;
        ox = 100;
        oy = -50;
        startHeading = pavHeading + pavHeadingOffset;
        %startHeading = 90;
end
N = ceil(Nsec/dt);  % total sim steps

switch syss
    case 'crossTrack'
        n = 4;  % STATES
    case 'crossTrack_integrator'
        n = 5;  % STATES
end
m = 1;  % CONTROL INPUT
q = 1;  % MEASUREMENTS


%% setup MPC params (THESE ARE IN PHYSICAL UNITS)

% max and mins for MPC
% hdmax = 30;
% hmax = 90;
% exmax = 100;
% epsimax = 90;

hdmax = 1000;
hmax = 90;
exmax = 500;
epsimax = 90;



% u is the change in heading setpoint.  (acts as a slew rate)
umax = 30*ones(m,1); umin = -umax;

slewRate = 45;   % deg per time step % not used now

Qmpc = eye(n);
Qmpc(1,1) = 0;  % no cost on ePSI
Qmpc(n,n) = 40; % cross-track error
Pfac = 10;
switch syss
    case 'crossTrack'
        xmax = [epsimax hdmax hmax exmax]'.*ones(n,1);xmin=-xmax;
    case 'crossTrack_integrator'
        % integrator saturation: (scaled by Cd later)
        intSat = 10*dt;   % m*sec  
        % mpc max: (also scaled by Cd later)
        intmax = intSat*10;
        xmax = [epsimax intmax hdmax hmax exmax]'.*ones(n,1);xmin=-xmax;
        Qmpc(2,2) = 1e-2; % cost on integral term
end
Pmpc = Pfac*Qmpc;

% constrain perpendicular speed to be fraction of speed*dt
% (half: max 30 deg heading diff) Taylor 1st order |error|=0.0236
% (1/3: max 20 deg heading diff) Taylor 1st order |error|=0.007
angle2speed = 1/2;
%angle2speed=1;

%% generate A, B matrices

s = tf('s');
Kcross=hspeed*pi/180;
wnH = 1;
%wnH = 1/3;
%wnH = 4;

zetaH = .8;
%tauRudder = 0.25;
TFrudder=1;
% this version just uses stable 2nd order for heading
CLheading = wnH^2/(s^2+2*wnH*zetaH*s+wnH^2)*TFrudder;
crossTrack = CLheading*Kcross/s;

switch syss
    case 'crossTrack'
        
        sysC = crossTrack;
        [num den] = tfdata(sysC);
        [Ac3,Bc3,~,Dc] = tf2ss(num{1},den{1});
        Cc = [1 0 0 0;0 1 0 0;0 0 wnH^2 0;0 0 0 hspeed*wnH^2*pi/180];
        % add commanded heading state
        Ac = zeros(n);
        Ac(2:n,2:n) = Ac3;      % closed-loop heading
        Ac(n,n-1) = -1;           % sign convention for xtrack
        Ac(2,1) = 1;            % CL heading setpoint 
        
        % Bc is input matrix for dPSI (change in setpt for CL heading)
        Bc = [1/dt 0 0 0]';
   
    case 'crossTrack_integrator'
        
        sysC = crossTrack;
        [num den] = tfdata(sysC);
        [Ac3,Bc3,~,Dc] = tf2ss(num{1},den{1});
        Cc = [1 0 0 0 0;0 speed*wnH^2*pi/180 0 0 0;0 0 1 0 0;0 0 0 wnH^2 0;0 0 0 0 hspeed*wnH^2*pi/180];

        % add commanded heading state
        Ac = zeros(n);
        Ac(3:n,3:n) = Ac3;      % closed-loop heading
        Ac(n,n-1) = -1;           % sign convention for xtrack
        Ac(3,1) = 1;            % CL heading setpoint 
        % add integral of cross-track state
        % (uses dt)
        Ac(2,n) = 1;            % integrator of xtrack error
        
        % Bc is input matrix for dPSI (change in setpt for CL heading)
        Bc = [1/dt 0 0 0 0]';
        
end
% Bin is input matrix for desired states
Bin = eye(n);

sysCss=ss(Ac,Bc,Cc,Dc);
% discrete-time:
sysd = c2d(sysCss,dt);     
[Ad Bd CdAll Dd] = ssdata(sysd);    % this uses full state output
% Cd is for just CROSS-TRACK ERROR MEASUREMENT
Cd = CdAll(n,:);
% repeat to get setpoint input
sysdSetpt = c2d(ss(Ac,Bin,Cc,Dc),dt);
[~,Bdin,~,~] = ssdata(sysdSetpt);

% convert z, max/min, Qmpc to discrete time states
if(strcmp(syss,'crossTrack_integrator'))
    intSat = intSat/CdAll(2,2);
end
xmax = CdAll\xmax; xmin = CdAll\xmin;
Qmpc = Qmpc.*CdAll.^2;
Pmpc = Pmpc.*CdAll.^2;
Qhalf = sqrtm(Qmpc);

% setup structures to pass to KF, MPC
sys=struct('n',n,'m',m,'Ad',Ad,'Bd',Bd,'Cd',Cd,...
    'dt',dt,'Bdin',Bdin);
MPCparams=struct('Qhalf',Qhalf,'Pmpc',Pmpc,'mu',mu,'T',T,...
    'ifQuiet',ifQuiet,...
    'speed',speed,'angle2speed',angle2speed,'slewRate',slewRate,...
    'xmax',xmax,'xmin',xmin,'umax',umax,'umin',umin,'CdAll',CdAll);

%% compute desired trajectory
% (in generateTracklinesMPC)
generateTracklinesMPC

%% plot step responses
%{
    tmax=dt*5;stepin=15;  % deg, input
    [y t1] = step(headingRate,tmax);[y,t2] =step(heading,max(t1));
    [y,tc]= step(crossTrack,max(t1));[y_d,td] = step(sysd,max(tc));
    figure
    colorvec={'g--.','b--.','r--.','k--.'};
    for i = 1:n
        stairs(td,stepin*y_d(:,i),colorvec{i})
        xlabel('sec');hold on
    end
    legend('heading accel','heading rate','heading','cross-track')
    figure;subplot(1,2,1)
    step(sysCss*15,tmax)
    hold on;step(sysd*15,tmax);subplot(1,2,2);step(sysd*15,tmax)
%}


