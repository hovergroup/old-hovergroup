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
- 8/20/2012: changed sim to use error (and add in desired) to match KF and
            real sys
    - fixed CL heading system
- 8/31/2012: updated with crossTrack and crossTrack_integrator systems,
    removed KFDelay, loss2MPC (for now)
    - changed KF input to be u - dDesHeading
- 9/1/2012: added integrator saturation, moved some setup here from
    configKayak (keep sim and MOOS separate)
- 9/2/2012: zero integrator state at turns 


%}


%setup

clear all;
%close all;
clc
format compact
% run configuration script for parameters and system
configureKayakMPC

%% parameters and configuration for kayak kf/mpc simulation
simNoise=0;         % process + meas noise in sims (always in kf)
wOffset = zeros(n,1);   % nonzero mean noise
wOffset(n-1) = 0;

% SIM initial state (ACTUAL BEARING): (IN PHYSICAL UNITS)
% [headingAccel(deg/s^2), headingRate(deg/s), heading(deg), crossTrack (m)]
heading0 = 73+10;
%heading0 = 185;
ex0 = -5;
switch syss
    case 'crossTrack'
        x0true = [heading0;0;heading0;ex0];
        simDes = [desBearing;zeros(size(desBearing));desBearing;zeros(size(desBearing))];
        simDes = [simDes(:,1) simDes(:,1:end)];
        dDesHeading = desBearing(2:end) - desBearing(1:end-1);
        dDesHeading = [0 dDesHeading(1:end)];
    case 'crossTrack_integrator'
        x0true = [heading0;0;0;heading0;ex0];
        simDes = [desBearing;zeros(size(desBearing));zeros(size(desBearing));desBearing;zeros(size(desBearing))];
        simDes = [simDes(:,1) simDes(:,1:end)];
        dDesHeading = desBearing(2:end) - desBearing(1:end-1);
        dDesHeading = [0 dDesHeading(1:end)];
end

% KF process noise, cross-track:
%Qcross=1e-2;    % continuous time PSD
%Qheading=1e-2;
%Qcross = 5;
%Qheading = 5;
%RGPS=3;           % GPS var.

RGPS = 3;      % m squared
Qheading = 5;   % deg squared
Qcross = 5;    % m squared

ifSave=0;           % save .mat and .txt of command window
uniques=sprintf('test_%dloss_prevNext',floor(probPLoss*100));   % unique name to append to filename
initializeMPCSim

%% run simulation
simStart=tic;
for i = 1:(N)
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % measure + estimate state

    if(simNoise);v = (randn(1,q)*sqrt(Rkf))';end
    z = Cd*xd + v;
    
    % convert z into an meas error,
    if(i>1)
        ez = z.*sin(deg2rad(90 - dDesHeading(i)));
    end
    % wrap ez(1) to +/- 180 deg
    if(ez(1) > 180);ez(1) = ez(1) - 360;end
    if(ez(1) < (-180));ez(1) = ez(1) + 360;end
    
    % run kf based on z(i+1) (based on e(i)), u(i), and ehat(i)
    % dDesHeading is for coord xform at turns, outputs ehat(i+1)
    if(i>1)
        %*********** (problems with sim - should be OK with real sys)
        
        % this gives better plans, but sim/KF a bit off (causes OS):
        %ddH = dDesHeading(i-1);
        % this more accurate, but plans a bit off near turn:
        ddH = dDesHeading(i);
    else
        ddH = 0;
    end
    uKF = u - ddH;
    [ehat PKF] = kayakKF(sys,KFparams,ez,ehat,PKF,uKF,ddH);
    
    % integrator saturation
    if(strcmp(syss,'crossTrack_integrator'))
        if(ehat(2)>(intSat))
            ehat(2) = intSat;
        elseif(ehat(2)<(-intSat))
            ehat(2) = -intSat;
        end
        %zero integral at turns
        if(i<N)
            if(dDesHeading(i+1))
                ehat(2) = 0;
            end
        end
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % propagate true system using u(i) and x(i)
    xhat = ehat + simDes(:,i);
    if(xhat(n-1)<0);xhat(n-1) = xhat(n-1)+360;end
    if(xhat(n-1)>360);xhat(n-1) = xhat(n-1) - 360;end
    switch syss
        case 'crossTrack'
            simxform = [1 1 1 sin(deg2rad(90 - dDesHeading(i)))];
        case 'crossTrack_integrator'
            simxform = [1 1 1 1 sin(deg2rad(90 - dDesHeading(i)))];
    end
    
    % generate noise for propagation (after KF)
    if(simNoise)
        w = sqrt(Qkfd)*randn(n,1) + wOffset;
        wvec = diag(w)*ones(n,nc);
    end
    
    % simulation coord frame xform (from step i)
    ed = (xd.*simxform' - simDes(:,i));
    
    % wrap ed(n-1) to +/- 180 deg
    if(ed(n-1) > 180);ed(n-1) = ed(n-1) - 360;end
    if(ed(n-1) < (-180));ed(n-1) = ed(n-1) + 360;end
    
    % discrete-time:
    ed= Ad*ed + Bd*u + Bdnoise*w;
    
    % continuous-time:
    tspan=linspace(0,dt,nc+1);
    e0ode=(xcsim(:,nc+1)-simDes(:,i)).*simxform';
    if(e0ode(n-1) > 180);e0ode(n-1) = e0ode(n-1) - 360;end
    if(e0ode(n-1) < (-180));e0ode(n-1) = e0ode(n-1) + 360;end
    [t_out,ecsim]=ode45(@(t,x) kayak_continuous(t,x,Ac,Bc,Bnoise,...
        u,wvec,tspan),tspan,e0ode);
    
    % add back desired
    xd = ed + simDes(:,i);
    
    % wrap xd and xcsim to 0,360
    if(xd(n-1)<0);xd(n-1) = xd(n-1)+360;end
    if(xd(n-1)>360);xd(n-1) = xd(n-1) - 360;end
    for j = 1:(nc+1);
        xcsim(:,j) = ecsim(j,:)' + simDes(:,i);
        if(xcsim(n-1,j)<0); xcsim(n-1,j) = xcsim(n-1,j) + 360;end
        if(xcsim(n-1,j)>360);xcsim(n-1,j) = xcsim(n-1,j) - 360;end
    end
    
    % integrator saturation
    if(strcmp(syss,'crossTrack_integrator'))
        if(xd(2)>(intSat));xd(2) = intSat;
        elseif(xd(2)<(-intSat));xd(2) = -intSat;end
        for j = 1:(nc+1);
            if(xcsim(2,j)<(-intSat)); xcsim(2,j) = -intSat;end
            if(xcsim(2,j)>360);xcsim(2,j) = intSat;end
        end
        if(i<N)
            if(dDesHeading(i+1))
                xd(2) = 0;
                xcsim(2,:) = 0;
            end
        end
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % run model predictive control
    % using xhat(i+1) - estimated by measuring x(i)
    xmpc = ehat;
    eDes = computeMPCInputs(n,N,T,desBearing,i);
    disp(eDes(n-1,:))
    
    %%%%%%%%%
    eDes = zeros(size(eDes));
    %%%%%%%%%
    
    if(i>1)
        % MPC knows previous control IT COMPUTED
        %uPrev = uPlanNext(1)+dDesHeading(i);
        uPrev = uPlanNext(1);
    else
        uPrev = 0;
    end
    fprintf('MPC Previous control input: %f \n',uPrev);
    
    if(uDelay)
        [uPlanNext tmpc X] = solveKayakMPC(sys,xmpc,MPCparams,uDelay,uPrev,eDes);
    else
        [uPlan tmpc X] = solveKayakMPC(sys,xmpc,MPCparams,uDelay,uPrev,eDes);
    end
    
    % (encode, quantize plan)
    
    % SIM PACKET LOSS (WITH DELAY)
    % buffer works on uPlan: RECEIVED PLAN AT THIS TIME STEP (delayed)
    [uPlanBuffered kPlan ifPLoss] = simPacketLossMPC...
        (uPlan,uPlanBuffered,kPlan,probPLoss);
    u = uPlanBuffered(:,kPlan);     % send appropriate control action
    
    %fprintf('Current computed plan;Buffered Plan \n')
    if(uDelay)
        disp(uPlanNext)
        %fprintf('Previous computed plan: \n')
    end
    %disp(uPlan)
    %fprintf('Buffered plan: \n')
    %disp(uPlanBuffered)
    
    switch syss
        case 'crossTrack'
            uBearing = ehat(1) + desBearing(i) + u;
        case 'crossTrack_integrator'
            uBearing = ehat(1) + desBearing(i) + u;
    end
    if(uBearing<0);uBearing = uBearing+360;end
    if(uBearing>360);uBearing = uBearing - 360;end
    fprintf('\n Next step heading controller setpoint: %f\n\n',uBearing)
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % save full vectors
    % meas/estimate values are for next step in time (delayed)
    vAll(:,i+1) = v;zAll(:,i+1) = z;
    xAllEst(:,i+1) = xhat;pKFall(:,:,i+1) = PKF;
    
    % control computed at this step, but applied next step (delayed)
    uSave{i+1} = uPlan;
    XMPCSave{i+1} = X;
    uAllMPC(:,i+1)=u;
    timeMPCall(i) = tmpc;
    ifPLossAll(i+1) = ifPLoss;
    
    % propagation happens this step
    wAll(:,i) = w;      % process noise and propagation for next step
    xAllTrueD(:,i) = xd;
    xAllTrueC(:,(nc*(i-1)+1):(nc*(i))) = xcsim(:,1:nc);
    tAllTrueC((nc*(i-1)+1):(nc*(i))) = t_out(1:nc);
    fprintf('\n')
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
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
if(ispc)
    cd(old)
end
