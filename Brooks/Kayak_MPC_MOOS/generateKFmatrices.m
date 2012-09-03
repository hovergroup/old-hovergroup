% creates kayak open-loop model for use with KF
% includes integrator state, 2nd order heading rate, integrators for 
% heading and cross-track.  
% 
% writes to matrices.txt in ivp-extend/{brooks or josh}/missions


% BR, 9/2/2012

clear all;close all;clc


%sys = 'crossTrack';n = 4;
sys = 'crossTrack_integrator';n = 5;

%kayak = 'kassandra_modem_smallR';
%kayak = 'kassandra_modem_30R';
kayak = 'nostromo_modem';

Rcompass = 0.5;       % compass var
RGPS=4;            % GPS var.
Qheading = 9;
Qcross = 16;

% kayak open-loop cross-track model
switch kayak
    case 'nostromo_modem'
        Krate=1/1.56; % rudder in to heading rate out
        wn=sqrt(1.56);
        zeta=1.01/(2*wn);
        speed=1.5; % m/s
    case 'kassandra_modem_smallR'
        Krate = 1.37/1.13;
        wn = sqrt(1.13);
        zeta = 0.5/(2*wn);
        speed = 0.8;
    case 'kassandra_modem_30R'
        Krate = 1.19/1.13;
        wn = sqrt(1.13);
        zeta = 1.09/(2*wn);
        speed = 0.8;    % with 65% thrust...?
end

dt = 1/5;
s = tf('s');
heading = (Krate*wn^2)/(s*(s^2+2*wn*zeta*s+wn^2));
Kcross=speed*pi/180;
crossTrack = heading*Kcross/s;
sysC = crossTrack;
[num den] = tfdata(sysC);
[Ac4,Bc4,Cc4,Dc] = tf2ss(num{1},den{1});

switch sys
    case 'crossTrack'
        Ac = Ac4;
        % xtrack error sign
        Ac(n,n-1) = -Ac(n,n-1);
        Bc = Bc4;
        CcAll = [1 0 0 0;0 1 0 0;0 0 Krate*wn^2 0;0 0 0 Kcross];
        Bc = [1 0 0 0]';
        Bnoise=eye(n);  % process noise input gain

    case 'crossTrack_integrator'
        
        Ac = zeros(n);
        % open-loop heading
        Ac(2:n,2:n) = Ac4;        
        % xtrack error sign
        Ac(n,n-1) = -Ac(n,n-1);
        % add integral of xtrack error
        Ac(1,n) = 1;
        Bc = [0;Bc4];
        CcAll = [1 0 0 0 0;0 1 0 0 0;0 0 1 0 0;0 0 0 Krate*wn^2 0;0 0 0 0 Kcross];
        Bnoise = eye(n);
        Bnoise(1,1) = 0;
        
end

sysCKF=ss(Ac,Bc,CcAll,Dc);

% discrete-time:
sysd = c2d(sysCKF,dt);
[Ad Bd CdAll Dd] = ssdata(sysd);    % this uses full state output

% Cd is for heading and cross-track error measurements
Cd = [CdAll(n-1,:);CdAll(n,:)];

% KF params
Qkfc=zeros(n);
Qkfc(n-1,n-1)=Qheading;
Qkfc(n,n)=Qcross;
Qkfd = Qkfc/dt;
% noise in heading and cross-track (correlation...)?
Qkfd = Qkfd/(CdAll.^2);

% measurement noise:
Rkf=[Rcompass 0;0 RGPS];

% repeat to get noise input
sysdNoise = c2d(ss(Ac,Bnoise,CcAll,Dc),dt);
[~,Bdnoise,~,~] = ssdata(sysdNoise);


% output to matrices.txt

Ad
Bd
Bdnoise
Cd
Qkfd
Rkf

fname='matrices';
%fpath='/home/brooks/hovergroup/Brooks/Kayak_MPC_MOOS/';
fpath='/home/brooks/hovergroup/ivp-extend/brooks/missions/';
fid = fopen([fpath,fname,'.txt'],'w');
printMatrix(fid,Ad);
fprintf(fid,'\n');
printMatrix(fid,Bd);
fprintf(fid,'\n');
printMatrix(fid,Bdnoise);
fprintf(fid,'\n');
printMatrix(fid,Cd);
fprintf(fid,'\n');
printMatrix(fid,Qkfd);
fprintf(fid,'\n');
printMatrix(fid,Rkf);
fprintf(fid,'\n');
fclose(fid);


