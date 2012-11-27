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
- 10/9/2012 - modified/simplified for 2nd round of exps
    - removed some unused settings
    - changed so ePsi not in state-space system
    - (ePsi still in xmin/max)
- 10/15/2012 - switched Cd in sys to CdAll, added KF_cross
        
%}

ifQuiet = 1;          % if cvx is run in quiet mode
uDelay = 1;

% packet loss probability:
probPLoss = 0;
%probPLoss = 0;


%% PARAMETERS

% EFFECTIVELY THE SPEED ALONG TRACKLINE
% used for estimating length to waypoint
% SHOULD MATCH SPEED GIVEN TO pKalmanFilter
% (irrelevant when just doing one line)
speed = 2;    % nostromo modem
% speed = 0.8;  % kassandra modem

% SPEED USED IN THE LINEARIZED KINEMATIC CROSS-TRACK MODEL
%hspeed = 1.5;%KF_cross = 0.0262; % 1 m/s
hspeed = 2;KF_cross = 0.0349;    % 2 m/s
%%%

% Time step (sec)
%dt = 6;    % WIFI
dt = 7;     % true acomms


% Planning horizon (steps)
T=32;

mu=1;              % sparse control weight

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% set up a short trackline after the first waypoint (stop within this...)
tracklineType='straight';
Nsec = 30;N = ceil(Nsec/dt);  % total sim steps
%ox = -160;
%oy = -160;


ox = 940;
oy = 50;
%ox = -84;
%oy = -120;



%ox = 600;
%oy = 0;

% pavHeading = 73;
startHeading = 80;
%%%%%%%%%%%%%%%%%%%%%%%%%%%

% CL heading model order:
%nH = 2;
%nH = 5;
nH = 3;

%syss = 'crossTrack';
syss = 'crossTrack_integrator';
switch syss
    case 'crossTrack'
        n = nH + 1;     % plus ex
    case 'crossTrack_integrator'
        n = nH + 1 + 1; % plus ex, intx
end
m = 1;  % CONTROL INPUT
q = 1;  % MEASUREMENTS

%% setup MPC params (THESE ARE IN PHYSICAL UNITS)

% MPC constraints for ePsi, but ePsi handled separately from A

% max and mins for MPC
hmax = 90;      % relative to trackline...
exmax = 500;    % in meters...cost fcn should handle this

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
epsimax = 30;   % MAIN CONSTRAINT: commanded heading within linear regime
% u is the change in heading setpoint.
% acts as a slew rate
umax = 30*ones(m,1); umin = -umax;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Qcross = 100;
Qheading = 0.1;
Qint = 0;   %.1;
Pfac = 10;

Qmpc = zeros(n+1);  % mpc has ePsi as a state
switch syss
    case 'crossTrack'
        xmax = [epsimax 1e5*ones(1,nH-1) hmax exmax]'.*ones(n+1,1);
        Qmpc(n+1,n+1) = Qcross;
        Qmpc(n,n) = Qheading;
    case 'crossTrack_integrator'
        % integrator saturation: (scaled by Cd later)
        intSat = 100*dt;   % m*sec
        % mpc max: (also scaled by Cd later)
        intmax = intSat*1.1;
        xmax = [epsimax 1e5*ones(1,nH-1) hmax exmax intmax]'.*ones(n+1,1);
        Qmpc(n,n) = Qcross;
        Qmpc(n-1,n-1) = Qheading;
        Qmpc(n+1,n+1) = Qint;
end
xmin=-xmax;
Pmpc = Pfac*Qmpc;

%% generate A, B matrices

s = tf('s');
Kcross=hspeed*pi/180;


switch nH
    case 2
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % 2nd order CL heading model
        wnH = 1;
        %wnH = 1/3;
        %wnH = 4;
        zetaH = .6;
        %tauRudder = 0.25;
        TFrudder=1;
        % this version just uses stable 2nd order for heading
        CLheading = wnH^2/(s^2+2*wnH*zetaH*s+wnH^2)*TFrudder;
        KH = 1; % DC gain of CL heading
        C_KH = wnH^2;
    case 3
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % 3rd order CL heading model (from sysID on simulated PID)
        %CLheading = (0.6433*s + 0.1135)/(s^3 + 1.681*s^2 + 0.5983*s + 0.1135);
        % 3rd order CL heading model (from sysID on 3 step responses, eric)
        A = 2.4;B=3.5;C=4.3;D=2.4;
        CLheading = A/(s^3+B*s^2+C*s+D);
        
        KH = 1;
        C_KH = A;
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end

crossTrack = CLheading*Kcross/s;

switch syss
    case 'crossTrack'
        
        sysC = crossTrack;
        [num den] = tfdata(sysC);
        [Ac3,Bc3,~,Dc] = tf2ss(num{1},den{1});
        
        Cc = zeros(2,n);
        Cc(1,n-1) = C_KH;
        Cc(2,n) = C_KH*Kcross;
        
        Ac = zeros(n);
        Ac(1:n,1:n) = Ac3;
        Ac(n,n-1) = -1;     % sign convention for xtrack
        
        Bc = [1 zeros(1,n-1)]';
        
    case 'crossTrack_integrator'
        
        sysC = crossTrack;
        [num den] = tfdata(sysC);
        [Ac3,Bc3,~,Dc] = tf2ss(num{1},den{1});
        
        Cc = zeros(3,n);
        Cc(1,n-2) = C_KH;
        Cc(2,n-1) = C_KH*Kcross;
        Cc(3,n) = C_KH*Kcross;
        
        % add commanded heading state
        Ac = zeros(n);
        Ac(1:n-1,1:n-1) = Ac3;      % closed-loop heading
        Ac(n-1,n-2) = -1;           % sign convention for xtrack
        Ac(n,n-1) = 1;                % integrator of xtrack error
        
        Bc = [1 zeros(1,n-1)]';
        
end
% Bin is input matrix for desired states
% Bin = eye(n);

sysCss=ss(Ac,Bc,Cc,Dc);
% discrete-time:
sysd = c2d(sysCss,dt);
[Ad Bd CdOut Dd] = ssdata(sysd);    % this uses full state output
% CdAll has heading, cross-track, (intx) outputs
% Cd is for just CROSS-TRACK ERROR MEASUREMENT
switch syss
    case 'crossTrack_integrator'
        Cd = [zeros(1,n-2) C_KH*Kcross 0];
        intSat = intSat/(C_KH*Kcross);
        CdAll = [zeros(nH-1,n);CdOut];
        CdAll(1:nH-1,1:nH-1) = diag(ones(1,nH-1));
    case 'crossTrack'
        Cd = [zeros(1,n-1) C_KH*Kcross];
        CdAll = [zeros(nH-1,n);CdOut];
        CdAll(1:nH-1,1:nH-1) = diag(ones(1,nH-1));
end

xmax(2:end) = CdAll\xmax(2:end); xmin(2:end) = CdAll\xmin(2:end);
Qmpc(2:end,2:end) = Qmpc(2:end,2:end).*CdAll.^2;
Pmpc(2:end,2:end) = Pmpc(2:end,2:end).*CdAll.^2;
Qhalf = sqrtm(Qmpc);

% setup structures to pass to KF, MPC
sys=struct('n',n,'m',m,'Ad',Ad,'Bd',Bd,'CdAll',CdAll,...
    'dt',dt);
MPCparams=struct('Qhalf',Qhalf,'Pmpc',Pmpc,'mu',mu,'T',T,...
    'ifQuiet',ifQuiet,'speed',speed,...
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
%


%
[yd,td] = step(sysd,30);
[yc,tc] = step(sysCss,30);
figure
subplot(2,1,1)
stairs(td,yd(:,1))
hold on
plot(tc,yc(:,1),'r')
ylabel('deg')
title('Heading Error')

subplot(2,1,2)
stairs(td,yd(:,2))
hold on
plot(tc,yc(:,2),'r')
legend('discrete','continuous')
ylabel('meters')
xlabel('Time [sec]')
title('Cross-Track Error')

%}

