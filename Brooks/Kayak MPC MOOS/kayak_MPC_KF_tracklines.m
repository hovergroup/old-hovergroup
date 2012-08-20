% more complex MPC kayak sim - with trajectory

% BR, 8/6/2012

% includes kf, full continuous-time simulation with noise

% to add:
% -encoding/quantization?
% -kf (or if) delays?

% ** still a bug with 0-360 wrap and cross-track (end of hexagon)

% changes
%{
- 8/13/2012: added variable setpoint (desired heading) to mpc
- 8/16/2012: changed to use error, define error based on xdes
- 8/19/2012: fixed some indexing issues with desBearing
    - added fcn computeMPCInputs, plotMPCSims
    - added simulation of packet loss, control packet buffering
    - changed cont. time sim to use same process noise as discrete
    - added mods to support different systems
            (state n-1 = heading, state n = xtrack)
    - changed '3' to 'n-1' for heading state (work with both sys)


%}


%setup

clear all;
%close all;
clc
format compact

% parameters and configuration for kayak kf/mpc simulation
simNoise=1;         % process + meas noise in sims (always in kf)
KFdelay=0;          % meas available immediately or with delay

ifSave=0;           % save .mat and .txt of command window
unique='';   % unique name to append to filename

% run configuration script for parameters and system
configureKayakMPC

%% save filename
% note - this setup for tvec = length 1
%savefilename = sprintf('%s_kayakmpcsim_dt%s_t%d_mu_%s_%s',datestring('dhms'),...
%    printnumfile(dt,2),t,printnumfile(mu,2),unique);
saveFilename = '';
if(ifSave)
    diary([saveFilename '_log.txt'])
end

%% run simulation

% preallocate
xAllTrueD = zeros(n,N);xAllTrueC = zeros(n,N*nc);tAllTrueC = zeros(1,N*nc);
xAllEst = zeros(n,N+1);pKFall = zeros(n,n,N+1);
uAllMPC = zeros(m,N+1);uSave = cell(N+1,1);
for i=1:N+1;uSave{i}=zeros(m,T);end
ifPLossAll = zeros(1,N);
timeMPCall = zeros(N,1);
vAll = zeros(q,N);zAll = zeros(q,N);wAll = zeros(n,N);

% intialize
simStart=tic;
x = x0;xd = x0;xcsim = repmat(Cc\x0c,1,nc);xhat = x0;
u = zeros(m,1);
w = zeros(n,1);wvec = zeros(n,nc);v=zeros(q,1);
PKF = eye(n);
xDes = zeros(n,T+2);xDes(n-1,:)=desBearing(1:T+2);
eDes = zeros(n,T+2);ehat = x0 - xDes(:,1);

switch syss
    case 'crossTrack'
        ez = [(x0c(n-1) - desBearing(1)) 0]';
        simDesIn = [0 0 desBearing(1) 0];
        xhat = ehat+simDesIn';dDesHeading = 0;
    case 'crossTrack_CLheading'
        ez = x0c(n);simDesIn = [0 desBearing(1) 0];
        xhat = ehat+simDesIn';dDesHeading = 0;
end

if(uDelay);uPlan=zeros(m,T);uSave{2}=uPlan;
else uPlan=zeros(m,T);end
if(KFdelay)
    if(strcmp(syss,'crossTrack'))
        ezold = (Cd*x0)-[desBearing(1) 0]';
    elseif(strcmp(syss,'crossTrack_CLheading'))
        ezold = (Cd*x0);
    end
    if(ezold(1) > 180);ezold(1) = ezold(1) - 360;end
    if(ez(1) < (-180)); ezold(1) = ezold(1) + 360;end
end
fp=0;fm=0;
kPlan = 1;  % no init packet loss
uPlanBuffered = uPlan;

% save full vectors
xAllEst(:,1) = xhat;pKFall(:,:,1) = PKF;
uAllMPC(:,1)=u;uSave{1}=uPlan;
timeMPCall(1) = 0;

simStart=tic;
for i = 1:(N)
    
    % start loop with previous state measurement (meas of x(i))
    % compute change in desired heading
    if(i>1)
        dDesHeading = desBearing(i)-desBearing(i-1);
        if(dDesHeading < (-180));dDesHeading = ((360-desBearing(i-1))+desBearing(i));end
        if(dDesHeading > (180));dDesHeading = desBearing(i)-(360-desBearing(i-1));end
    end
    
    % measure true state (with noise)
    if(simNoise);v = (randn(1,q)*sqrt(Rkf))';end
    z = Cd*x + v;
    
    % convert z into an meas error
    switch syss
        case 'crossTrack'
            if(i>1)
                ez = z.*[1 sin(deg2rad(90 - dDesHeading))]' - [desBearing(i) 0]';
            end
            % wrap ez(1) to +/- 180 deg
            if(ez(1) > 180);ez(1) = ez(1) - 360;end
            if(ez(1) < (-180));ez(1) = ez(1) + 360;end
        case 'crossTrack_CLheading'
            if(i>1)
                ez = z.*sin(deg2rad(90 - dDesHeading));
            end
    end
    
    % run kf based on z(i+1) (based on x(i)), u(i), and xhat(i)
    % outputs xhat(i+1)
    if(KFdelay)
        [ehat PKF] = kayakKF(sys,KFparams,ezold,ehat,PKF,u,dDesHeading);
        ezold = ez;
        % encode option for mult. time step delay?
    else
        [ehat PKF] = kayakKF(sys,KFparams,ez,ehat,PKF,u,dDesHeading);
    end
    
    % propagate true system using u(i) and x(i)
    switch syss
        case 'crossTrack'
            if(i>1)
                xhat = ehat + [0 0 desBearing(i-1) 0]';
                if(xhat(n-1)<0);xhat(n-1) = xhat(n-1)+360;end
                if(xhat(n-1)>360);xhat(n-1) = xhat(n-1) - 360;end
                simDesIn = [0 0 desBearing(i-1)+dDesHeading 0];
            end
            simxform = [1 1 1 sin(deg2rad(90 - dDesHeading))];
        case 'crossTrack_CLheading'
            if(i>1)
                xhat = ehat + [0 desBearing(i-1) 0]';
                if(xhat(n-1)<0);xhat(n-1) = xhat(n-1)+360;end
                if(xhat(n-1)>360);xhat(n-1) = xhat(n-1) - 360;end
                simDesIn = [0 desBearing(i-1)+dDesHeading 0];
            end
            simxform = [1 1 sin(deg2rad(90 - dDesHeading))];
    end
    
    % generate noise for propagation (after KF)
    if(simNoise)
        w = sqrt(Qkfd)*randn(n,1);
        wvec = diag(w)*ones(n,nc);%sqrt(qkfc)*randn(n,nc)/sqrt(dt/nc);
    end
    
    
%     if(i>49)
%         i
%         simDesIn
%         dDesHeading
%         simxform
%     end
    
    
    
    % simulation with coord frame xform
    xd = xd.*simxform';
    % discrete-time:
    xd = Ad*xd + Bd*u + Bdin*simDesIn'+ Bdnoise*w;
    
    % continuous-time:
    tspan=linspace(0,dt,nc+1);
    if(i==1)
        x0ode=Cc\x0c;
    else
        x0ode=xcsim(:,nc+1).*simxform';
    end
    
%     % wrap cont time sim to +/-180, sim, then unwrap
%     fp=0;fm=0;
%     if(x0ode(n-1) > 180);
%         fm=1;x0ode(n-1) = x0ode(n-1) - 360;simDesIn(n-1)=simDesIn(n-1)-360;
%     end
%     if(x0ode(n-1) < (-180));
%         fp=1;x0ode(n-1) =x0ode(n-1) + 360;simDesIn(n-1)=simDesIn(n-1)+360;
%     end
    [t_out,xcsim]=ode45(@(t,x) kayak_continuous(t,x,Ac,Bc,Bnoise,Bin,...
        simDesIn',u,wvec,tspan),tspan,x0ode);
%     if(fp);for j=1:nc;xcsim(j,n-1)=xcsim(j,n-1)-360;end;end
%     if(fm);for j=1:nc;xcsim(j,n-1)=xcsim(j,n-1)+360;end;end
%     
    % wrap xd and xcsim to 0,360
    if(xd(n-1)<0);xd(n-1) = xd(n-1)+360;end
    if(xd(n-1)>360);xd(n-1) = xd(n-1) - 360;end
    for j = 1:nc;
        if(xcsim(j,n-1)<0); xcsim(j,n-1) = xcsim(j,n-1)+360;end
        if(xcsim(j,n-1)>360);xcsim(j,n-1) = xcsim(j,n-1) - 360;end
    end
    xcsim=xcsim';x = xd;
    
    % run model predictive control
    % using xhat(i+1) - estimated by measuring x(i)
    xmpc = ehat;
    eDes = computeMPCInputs(n,N,T,desBearing,i);
    disp(eDes(n-1,:))
    
    if(loss2MPC)
        % MPC knows previous command is the buffered value
        uPrev = u;
    else
        % MPC assumes previous command was the one it computed
        uPrev = uPlan(:,1);
    end
    
    if(uDelay)
        [uPlanNext tmpc] = solveKayakMPC(sys,xmpc,MPCparams,uDelay,uPlan(:,1),eDes);
    else
        [uPlan tmpc] = solveKayakMPC(sys,xmpc,MPCparams,uDelay,u,eDes);
    end
    
    % (encode, quantize plan)
    
    % SIM PACKET LOSS (WITH DELAY)
    % buffer works on RECEIVED PLAN AT THIS TIME STEP (delayed)
    [uPlanBuffered kPlan ifPLoss] = simPacketLossMPC...
        (uPlan,uPlanBuffered,kPlan,probPLoss);
    u = uPlanBuffered(:,kPlan);     % send appropriate control action
    %u = uPlan(:,1);
    
    fprintf('Current computed plan: \n')
    if(uDelay)
        disp(uPlanNext)
        fprintf('Previous computed plan: \n')
    end
    disp(uPlan)
    fprintf('Buffered plan: \n')
    disp(uPlanBuffered)
    fprintf('\nActual control applied: %f \n',u)
    
    % save full vectors
    % meas/estimate values are technically for next step in time (delayed)
    vAll(:,i+1) = v;zAll(:,i+1) = z;
    xAllEst(:,i+1) = xhat;pKFall(:,:,i+1) = PKF;
    % control computed at this step, but applied next step (delayed)
    uSave{i+1} = uPlan;          % this control is really applied next step
    uAllMPC(:,i+1)=u;timeMPCall(i) = tmpc;
    ifPLossAll(i+1) = ifPLoss;
    % propagation happens this step
    wAll(:,i) = w;      % process noise and propagation for next step
    xAllTrueD(:,i) = xd;
    xAllTrueC(:,(nc*(i-1)+1):(nc*(i))) = xcsim(:,1:nc);
    tAllTrueC((nc*(i-1)+1):(nc*(i))) = t_out(1:nc);
    
    if(uDelay)
        uPlan=uPlanNext;
    end
    
end

simtime=toc(simStart);
%%
plotMPCSims

if(ifSave)
    save(saveFilename)
    diary off
end
