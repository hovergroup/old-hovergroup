% configureKayakMPC

% Parameters and configuration for kayak KF/MPC simulation
ifQuiet=0;          % if cvx is run in quiet mode


%% PARAMETERS

% System Params
%n = 3, m=1;

%syss='heading';
n = 4;  % STATES
m = 1;  % CONTROL INPUT
q = 2;  % MEASUREMENTS

syss='crossTrack';

% time step for system discretization
%dt=.2;
%dt=1;
dt=0.5;
%dt=6;

% number of 'continuous-time' samples in one time step
nc=dt/(1e-1);

% SYS DELAY...
%sysDelay = dt;

Nsec=84;            % sim length (seconds) 
N = ceil(Nsec/dt);  % total sim steps

% MPC params
termPenalty=1;      % terminal penalty on (if 0, must end exactly at 0)
mu=10;              % sparse control weight
%T=ceil(N/2);        % horizon length 
T=4;

Qmpc = eye(n);         % state cost
Rmpc = eye(m);         % control cost
% maybe scale by P from Ricatti??
Pmpc = 10*eye(n);     % terminal state cost


% initial state: (IN PHYSICAL UNITS)
% [headingAccel(deg/s^2), headingRate(deg/s), heading(deg), crossTrack (m)]
x0c = [0;0;0;15];

% max/mins (IN PHYSICAL UNITS)
xmax= [20 20 90 2*x0c(4)]'.*ones(n,1);xmin=-xmax;
umax = 30*ones(m,1); umin = -umax;

% System params
% kayak cross-track model
Krate=1/1.56; % rudder in to heading rate out
wn=sqrt(1.56);
zeta=1.01/(2*wn);
speed=2; % m/s

%slew rate
slewRate=(30)*dt; % rad/s

% constrain perpendicular speed to be fraction of speed*dt
% (half: max 30 deg heading diff) Taylor 1st order |error|=0.0236
% (1/3: max 20 deg heading diff) Taylor 1st order |error|=0.007
angle2speed = 1/2;

% KF params
Bnoise=eye(n);  % process noise input gain
% simple cross-track only: 
%Qcross=1e-2;    % continuous time PSD
%Qheading=1e-2;
Qcross = 1;
Qheading = 3;

Qkfc=[0 0 0 0;0 0 0 0;0 0 Qheading 0;0 0 0 Qcross];
Qkfd = Qkfc/dt;
% noise in heading and cross-track (correlation...)?


% measurement noise:
RCompass=3;       % compass std dev.
RGPS=5;           % GPS std dev.   


Rkf=[RCompass 0;0 RGPS];



%% generate A, B matrices

s = tf('s');
K=wn^2*Krate;
headingRate = (K/(s^2 + s + wn^2));
heading=headingRate/s;
% dz/dt = U*sin(theta) ~ U*theta (in radians)
Kcross=speed*pi/180;
crossTrack = heading*Kcross/s;
switch syss
    case 'crossTrack'
        sysC=crossTrack;
    case 'heading'
        sysC=heading;
end
% set up system
[num den] = tfdata(sysC);
% [num den] = tfdata(sys*exp(-s*sysDelay));

[Ac Bc Cc Dc] = tf2ss(num{1},den{1});
Cc = [1 0 0 0;0 K 0 0;0 0 1 0;Cc];

sysCss=ss(Ac,Bc,Cc,Dc);
sysd = c2d(sysCss,dt);

[Ad Bd CdAll Dd] = ssdata(sysd);    % this uses full state output
Cd = [0,0,CdAll(3,3),0;0,0,0,CdAll(4,4)];   % this for MEASUREMENT

% repeate to get noise input
sysdNoise = c2d(ss(Ac,Bnoise,Cc,Dc),dt);
[~,Bdnoise,~,~] = ssdata(sysdNoise); 




% convert z, max/min to discrete time states
x0 = CdAll\x0c;
xmax = CdAll\xmax; xmin = CdAll\xmin;
Qhalf = sqrtm(Qmpc); Rhalf = sqrtm(Rmpc);

% setup structures to pass to KF, MPC
sys=struct('n',n,'m',m,'Ad',Ad,'Bd',Bd,'Cd',Cd,'Bdnoise',Bdnoise,...
    'dt',dt);
MPCparams=struct('Qhalf',Qhalf,'Rhalf',Rhalf,'Pmpc',Pmpc,'mu',mu,'T',T,...
    'termPenalty',termPenalty,'ifQuiet',ifQuiet,...
    'speed',speed,'angle2speed',angle2speed,'slewRate',slewRate,...
    'xmax',xmax,'xmin',xmin,'umax',umax,'umin',umin,'CdAll',CdAll);
KFparams=struct('Rkf',Rkf,'Qkfd',Qkfd);
