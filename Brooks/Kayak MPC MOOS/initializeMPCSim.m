%initializeMPCSim

% BR, 8/29/2012

%{
-8/29/2012: separated from kayak_MPC_KF_tracklines to be cleaner
    changed eHat to subtract desBearing(1) from x0(1) (ePSI)
%}

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
if(strcmp(syss,'crossTrack_CLheading'))
    PKF(1,1) = 0;   % no uncert in heading setpoint...
end
xDes = zeros(n,T+2);
xDes(n-1,:)=desBearing(1:T+2);
eDes = zeros(n,T+2);


xd = x0;
xhat = x0;
xcsim = repmat(Cc\x0,1,nc+1);

switch syss
    case 'crossTrack'
        ehat = x0 - xDes(:,1);
        simDes = [0 desBearing(1) 0];
    case 'crossTrack_CLheading'
        ehat = x0 - [1 0 1 0]'*desBearing(1);
        simDes = [desBearing(1) 0 desBearing(1) 0];
end

switch syss
    case 'crossTrack'
        u = zeros(m,1); % initial rudder angle
        ez = [(Cc(n-1,n-1)*x0(n-1) - desBearing(1)) 0]';
    case 'crossTrack_CLheading'
        %u = x0(n-1) - desBearing(1); % initial (local) psi is straight
        u=0;
        ez = Cc(n,n)*x0(n);
end

dDesHeading = 0;
ehsim=0;ehcsim=0;

if(uDelay);uPlan=zeros(m,T);uSave{2}=uPlan;
else uPlan=zeros(m,T);end
if(KFdelay)
    if(strcmp(syss,'crossTrack'))
        ezold = (Cd*x0)-[desBearing(1) 0]';
        if(ezold(1) > 180);ezold(1) = ezold(1) - 360;end
        if(ez(1) < (-180)); ezold(1) = ezold(1) + 360;end
    elseif(strcmp(syss,'crossTrack_CLheading'))
        ezold = (Cd*x0);
    end
end

kPlan = 1;  % no init packet loss
uPlanBuffered = uPlan;

% save full vectors
xAllEst(:,1) = xhat;pKFall(:,:,1) = PKF;
uAllMPC(:,1)=u;uSave{1}=uPlan;
timeMPCall(1) = 0;