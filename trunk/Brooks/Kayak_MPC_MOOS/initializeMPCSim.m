%initializeMPCSim

% BR, 8/29/2012

%{
-8/29/2012: separated from kayak_MPC_KF_tracklines to be cleaner
    changed eHat to subtract desBearing(1) from x0(1) (ePSI)
- Added KF inits (moved from config), removed KFdelay
%}

%% setup some sim configurations

% KF params
Bnoise=eye(n);  % process noise input gain
Qkfc=zeros(n);
Qkfc(n-1,n-1)=Qheading;
Qkfc(n,n)=Qcross;
% noise in heading and cross-track (correlation...)?

if(strcmp(syss,'crossTrack_integrator'))
    %Qkfc(2,2)= Qcross;
    Qkfc(2,2) = Qcross;
end

Qkfd = Qkfc/dt;

% measurement noise:
Rkf=RGPS;

% number of 'continuous-time' samples in one time step
nc=dt/(1e-1);

Qkfd = Qkfd/(CdAll.^2);
x0 = CdAll\x0true;

% repeat to get noise input
sysdNoise = c2d(ss(Ac,Bnoise,Cc,Dc),dt);
[~,Bdnoise,~,~] = ssdata(sysdNoise);
KFparams=struct('Rkf',Rkf,'Qkfd',Qkfd,'Bdnoise',Bdnoise);


%% save filename

if(ispc)
    old = cd('C:\Brooks\Dropbox\Research Dropbox\MATLAB Code\Kayak MPC');
end
if(~exist('uniques','var'));uniques='';end
% note - this setup for tvec = length 1
saveFilename = sprintf('%s_kayakmpcsim_%s_%s_dt%s_T%d_mu_%s_%s',...
    dateString('DHMS'),syss,tracklineType,...
    printNumFile(dt,2),T,printNumFile(mu,2),uniques);
%saveFilename = '';
if(ifSave)
    diary([saveFilename '_log.txt'])
end

%% init sim

% preallocate
xAllTrueD = zeros(n,N);
xAllTrueC = zeros(n,N*nc);
tAllTrueC = zeros(1,N*nc);

xAllEst = zeros(n,N+1);
pKFall = zeros(n,n,N+1);

uAllMPC = zeros(m,N+1);
uSave = cell(N+1,1);
for i=1:N+1
    uSave{i}=zeros(m,T);
end
XMPCSave = cell(N+1,1);

ifPLossAll = zeros(1,N);
timeMPCall = zeros(N,1);
vAll = zeros(q,N);
zAll = zeros(q,N);
wAll = zeros(n,N);

% intialize
w = zeros(n,1);
wvec = zeros(n,nc);
v=zeros(q,1);

PKF = eye(n);
if(strcmp(syss,'crossTrack_integrator'))
    PKF(1,1) = 0;   % no uncert in integral?
end
eDes = computeMPCInputs(n,N,T,desBearing,1);

xd = x0;
xhat = x0;
xcsim = repmat(x0,1,nc+1);
ehat = x0 - simDes(:,1);
u = 0;
ez = Cc(n,n)*x0(n);
ehsim=0;ehcsim=0;

if(uDelay);uPlan=zeros(m,T);uSave{2}=uPlan;
else uPlan=zeros(m,T);end

kPlan = 1;  % no init packet loss
uPlanBuffered = uPlan;

% save full vectors
xAllEst(:,1) = xhat;pKFall(:,:,1) = PKF;
uAllMPC(:,1)=u;uSave{1}=uPlan;
timeMPCall(1) = 0;
