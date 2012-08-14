% More complex MPC kayak sim - WITH TRAJECTORY

% BR, 8/6/2012

% Includes KF, full continuous-time simulation with noise

% To add:
% Variable setpoint in MPC
% (packet loss?  IF?  encoding/quantization?)
% Delays (meas to KF, MPC to vehicle)


% changes
%{
- 8/13/2012: added variable setpoint (desired Heading) to MPC

%}


%SETUP

clear all;
%close all;
clc

% Parameters and configuration for kayak KF/MPC simulation
plotStep=0;
simNoise=0;         % process + meas noise in sims (always in KF)
KFdelay=0;          % meas available immediately or with delay
uDelay=1;

ifSave=0;           % save .mat and .txt of command window
unique='mu10_udelay';   % unique name to append to filename

% run configuration script for parameters and system
configureKayakMPC

%% save filename
% NOTE - this setup for Tvec = length 1
saveFilename = sprintf('%s_kayakmpcsim_dt%s_T%d_mu_%s_%s',dateString('DHMS'),...
    printNumFile(dt,2),T,printNumFile(mu,2),unique);

if(ifSave)
    diary([saveFilename '_log.txt'])
end

%% PLOT STEP RESPONSES
if(plotStep)
    
    nS=3;
    tmax=15;
    stepIn=15;  % deg, input
    
    [y t1] = step(headingRate,tmax);
    [y,t2]=step(heading,max(t1));
    [y,Tc]= step(crossTrack,max(t1));
    [y_d,Td] = step(sysd,max(Tc));
    
    % 3 step plots
    %{
figure
subplot(nS,1,1)
%plot(t,rad2deg(y),[0 max(t)],[1 1])
plot(t1,stepIn*(y));%,[0 max(t)],stepIn*[1 1],'k--')
title(sprintf('heading rate, input (for all) = %0.1f deg',stepIn))
ylabel('deg/s^2')
%legend('output','input')
subplot(nS,1,2)
plot(t2,stepIn*(y));%,[0 max(t)],stepIn*[1 1],'k--')
title('heading rate')
ylabel('deg/s^2')
title('heading')
ylabel('rad')
subplot(nS,1,nS)
plot(Tc,stepIn*y)
hold on
stairs(Td,stepIn*y_d(:,4),'r--.')
title('cross track error')
ylabel('m')
xlabel('sec')
legend('continuous','discrete','Location','NorthWest')
    %}
    
    % 4 states on 1 plot
    figure
    colorvec={'g--.','b--.','r--.','k--.'};
    
    for i = 1:4
        stairs(Td,stepIn*y_d(:,i),colorvec{i})
        xlabel('sec')
        hold on
    end
    legend('heading accel','heading rate','heading','cross-track')
    
    %
    figure
    subplot(1,2,1)
    step(sysCss*15,dt*2)
    hold on
    step(sysd,dt*2)
    subplot(1,2,2)
    step(sysd,dt*2)
    
    %}
    
end

%% Run simulation

% preallocate
XallTrueD = zeros(n,N);
XallTrueC = zeros(n,N*nc);
TallTrueC = zeros(1,N*nc);
XallEst = zeros(n,N);
PKFAll = zeros(n,n,N);
Uallmpc = zeros(m,N+1);
uSave = cell(N+1,1);
timeMPCAll = zeros(N,1);
vAll = zeros(q,N);
zAll = zeros(q,N);
wAll = zeros(n,N);

% intialize
simStart=tic;
x = x0;
xd = x0;
xcsim = repmat(Cc\x0c,1,nc);
xHat = ((x0));
u = zeros(m,1);
PKF = zeros(n,n);
xDes = zeros(n,T+2);xDes(3,:)=desBearing(1:T+2);

if(uDelay)
    uPlan=zeros(m,T);
    uSave{2}=uPlan;
else
    uPlan=zeros(m,T);
end

%if use delayed KF:
zOld = Cd*x0;

% save full vectors - t=1
%vAll(:,1) = 0;
%zAll(:,1) = 0;
XallEst(:,1) = xHat;
PKFAll(:,:,1) = PKF;
Uallmpc(:,1)=[u];      
uSave{1}=uPlan;
timeMPCAll(1) = [0];
% propagation handled at first step
%wAll(:,1) = zeros(n,1);
%XallTrueD(:,1) = x;
%XallTrueC(:,1:nc) = xcsim;

simStart=tic;

for i = 1:(N)
    
    % start loop with previous state measurement (meas of x(i))
    % measure true state (with noise)
    if(simNoise)
        v = (randn(1,q)*sqrt(Rkf))';
    else
        v = zeros(q,1);
    end
    z = Cd*x + v;
    % (could be a delay between measurement and KF/MPC too...)
    
    % run KF based on z(i+1) (based on x(i)), u(i), and xHat(i)
    % outputs xHat(i+1)
    if(KFdelay)
        [xHat PKF] = kayakKF(sys,KFparams,zOld,xHat,PKF,u,xDes(:,1));    
        zOld = z;
        % encode option for mult. time step delay?
    else
        [xHat PKF] = kayakKF(sys,KFparams,z,xHat,PKF,u,xDes(:,1));
    end

    
    % now propagate true system using u(i) and x(i)
    % process noise:
    if(simNoise)
        w = sqrt(Qkfd)*randn(n,1);
        wvec = sqrt(Qkfc)*randn(4,nc)/sqrt(dt/nc);
    else
        w = zeros(n,1);
        wvec = zeros(4,nc);
    end
    
    % discrete-time:
    xd = Ad*xd + Bd*u + Bdin*xDes(:,1) + Bdnoise*w;
    
    % continuous-time:
    tspan=linspace(0,dt,nc+1);
    if(i==1)
        x0ode=Cc\x0c;
    else
        x0ode=xcsim(:,nc+1);
    end
    [t_out,xcsim]=ode45(@(t,x) kayak_continuous(t,x,Ac,Bc,Bnoise,Bin,xDes(:,1),u,wvec,tspan),tspan,x0ode);
    xcsim=xcsim';
    % (nonlinear version with sin theta?)
    
    x = xd;
    
    % run model predictive control
    % using xHat(i+1) - estimated by measuring x(i)
    % outputs uPlan(i+1) (to be used next step)
    
    %[uPlan tMPC] = solveKayakMPC(sys,x,MPCparams);
    %[uPlan tMPC] = solveKayakMPC(sys,xHat,MPCparams);
    
    
    xMPC = x;
    u
    %xMPC = xHat;
    
    desiredHeading = desBearing(i);
    
    % THIS SIM DOESN'T DO BEARING CONVERSION...
    % map heading from [0,360] to [-180,+180]
%     errorHeading = desiredHeading - xMPC(3);
%     if(errorHeading>180)
%         errorHeading = errorHeading - 360;
%     end
%     if(errorHeading<-180)
%         errorHeading = errorHeading + 360;
%     end
%     xMPC(3) = errorHeading;
    
    xDes = zeros(n,T+2);
    if((i+T+1)<N)
        xDes(3,:) = desBearing(i:(i+T+1));
    else
        pad = N-i;
        xDes(3,:) = [desBearing(i:i+pad-1) desBearing(N-1)*ones(1,T+2-pad)];
    end
    xDes
    setPt=1;
    if(uDelay)
        [uPlanNext tMPC] = solveKayakMPC(sys,xMPC,MPCparams,uDelay,uPlan(:,1),setPt,xDes);
    else
        [uPlan tMPC] = solveKayakMPC(sys,xMPC,MPCparams,uDelay,u,setPt,xDes);
    end
    uPlan
    
    % (buffer control plan, sim packet loss?) select control
    u = uPlan(:,1);
    
    
    % save full vectors
    % meas/estimate values are technically for next step in time (delayed)
    vAll(:,i+1) = v;
    zAll(:,i+1) = z;
    XallEst(:,i+1) = xHat;
    PKFAll(:,:,i+1) = PKF;         
    % control computed at this step, but APPLIED next step (delayed)
    uSave{i+1} = uPlan;          % this control is really applied next step
    Uallmpc(:,i+1)=u;               
    timeMPCAll(i) = tMPC; 
    % propagation happens this step
    wAll(:,i) = w;      % process noise and propagation for next step
    XallTrueD(:,i) = xd;
    XallTrueC(:,(nc*(i-1)+1):(nc*(i))) = xcsim(:,1:nc);
    TallTrueC((nc*(i-1)+1):(nc*(i))) = t_out(1:nc);
    
    
        
    if(uDelay)
    uPlan=uPlanNext;
    end
    
    
end

simTime=toc(simStart);

%% print some parameters and results
fprintf('QMPC: \n')
disp(Qmpc)
fprintf('RMPC: \n')
disp(Rmpc)
fprintf('PMPC: \n')
disp(Pmpc)
fprintf('Slew Rate Constrained to: %0.1f deg/s\n',slewRate)
fprintf('Perpendicular speed = %0.2f Forward Speed \n',angle2speed)
fprintf('mu = %0.2f \n',mu)
fprintf('Sim noise Qkfd:\n')
disp(Qkfd)
fprintf('Sim meas noise Rkf:\n')
disp(Rkf)
fprintf('Initial State: \n')
disp(x0)
fprintf('N = %d steps, T = %d steps, dt = %0.2f sec \n\n',N,T,dt)

%% Plot MPC results

%%% add a little code that makes xticks (AND GRID) a multiple of the time
%%% step...(within some range)

tvec = linspace(0,N*dt,N+1);
tvecC = linspace(0,N*dt,(N)*nc+1);
figure;

% plot lower states, and saturations
%plotStates=1:n;
plotStates=[1 2 3];
for ii = plotStates;
    subplot(length(plotStates)+2,1,ii-min(plotStates)+1);
    set(gca,'Fontsize',16);
    stairs(tvec(1:end-1),CdAll(ii,:)*[XallTrueD],'k');
    hold on
    %stairs(tvec,CdAll(ii,:)*[XallEst [0;0;0;0]],'b');
    plot(tvec,CdAll(ii,:)*[XallEst],'rx')
    plot(tvecC(1:end-1),Cc(ii,:)*[XallTrueC],'b')
    ylabel(sprintf('x(%d)',ii));
    
end

%perp. speed (added constraint)
subplot(length(plotStates)+2,1,length(plotStates)+1)
stairs(tvec(2:(end-2)),(XallTrueD(4,2:(N-1))-XallTrueD(4,1:(N-2))).*CdAll(4,4)./dt,'k')
title('Speed perpendicular to trackline')
%xlabel('t [sec]')
ylabel('speed [m/s]')

subplot(length(plotStates)+2,1,length(plotStates)+2)
stairs(tvec(2:(end-2)),(Uallmpc(1,2:(N-1))-Uallmpc(1,1:(N-2)))./dt,'k')
title('Rudder slew rate')
xlabel('t [sec]')
ylabel('udot [deg/s]')

figure
% plot cross track error and control
subplot(2,1,1)
stairs(tvec(1:end-1),CdAll(4,:)*[XallTrueD],'k')
hold on
%stairs(tvec,CdAll(4,:)*[XallEst(:,:,1) [0 0 0 0]'],'b')
plot(tvec,CdAll(4,:)*[XallEst],'rx')
plot(tvecC(1:end-1),Cc(4,:)*[XallTrueC],'b')

set(gca,'Fontsize',16);
title({'Cross Track Error',...
    sprintf('T = %d steps, dt = %0.2f sec, mu = %0.1f,  ',T,dt,mu)});
ylabel('Cross Track Error [m]')
legend('Discrete','KF Estimate','Continuous')
subplot(2,1,2);
set(gca,'Fontsize',16);

stairs(tvec,[Uallmpc(1,1:(end))],'k');
title('Rudder Angle')
xlabel('t [sec]');
ylabel('u1 [deg]');


%% Plot all control plans...

figure
%colors=hsv(N);

%umaxs=umax;
umaxs=max(abs(Uallmpc));
stairs(0:N,[Uallmpc(1,1:N) 0]+2.2*umaxs,'b')
hold on
plot([0 N],2.2*umaxs*ones(1,2),'k--','LineWidth',1)
plot([0 N],[0 0],'k')

umaxs=0;
for i=2:N
    umaxs=max([umaxs max(uSave{i})]);
end

for i=2:N
    %stairs(1:T,uSave{i,1} - i*(2.2*umax),'Color',colors(i,:))
    %if(uDelay)
     %   stairs((i-1):(i-2+T-1),[uSave{i}] - (i-1)*(2.2*umaxs))
    %else
        stairs((i-1):(i-2+T),[uSave{i}] - (i-1)*(2.2*umaxs))        
    %end
    
    plot([0 N],-(i-1)*2.2*umaxs*ones(1,2),'k--','LineWidth',1)
    %plot(N-i,-(i*2.2*umax),'ko')
end
ylim = get(gca,'YLim');
axis([0 N ylim])
set(gca,'YTickLabel',[])
title('All control plans, actual is on top')
xlabel(sprintf('Time steps (each is %0.2f sec)',dt))


%%
if(ifSave)
    save(saveFilename)
    diary off
end
