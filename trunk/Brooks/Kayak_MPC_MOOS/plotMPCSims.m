% plot MPC Sim Results

% BR, 8/19/2012

% changes
%{
- 8/20/2012: made some changes to support both systems
- 9/1/2012: added plots for MPC plans and predictions at diff steps

%}

%% print some parameters and results
fprintf('qmpc: \n')
disp(Qmpc)
fprintf('pmpc: \n')
disp(Pmpc)
fprintf('slew rate constrained to: %0.1f deg/s\n',slewRate/dt)
fprintf('perpendicular speed = %0.2f forward speed \n',angle2speed)
fprintf('mu = %0.2f \n',mu)
fprintf('sim noise qkfd:\n')
disp(Qkfd)
fprintf('sim meas noise rkf:\n')
disp(Rkf)
fprintf('initial state: \n')
disp(x0)
fprintf('n = %d steps, t = %d steps, dt = %0.2f sec \n\n',N,T,dt)


%% plot mpc results

%%% add a little code that makes xticks (and grid) a multiple of the time
%%% step...(within some range)

tvec = linspace(0,N*dt,N+1);
tvecc = linspace(0,N*dt,(N)*nc+1);

packetLossInds = find(ifPLossAll);

figure;
% plot lower states, and saturations
%plotstates=1:n;
switch syss
    case 'crossTrack'
        titleCell = {'Setpoint','dHeading','Heading'};
    case 'crossTrack_integrator'
        titleCell = {'Setpoint','Cross-track integral','dHeading','Heading'};
end
plotstates=1:(n-1);
for ii = plotstates;
    subplot(length(plotstates)+1,1,ii-min(plotstates)+1);
    set(gca,'fontsize',16);
    stairs(tvec(1:end-1),CdAll(ii,:)*[xAllTrueD],'k');
    hold on
    %stairs(tvec,cdall(ii,:)*[xallest [0;0;0;0]],'b');
    plot(tvec,CdAll(ii,:)*[xAllEst],'rx')
    plot(tvecc(1:end-1),Cc(ii,:)*[xAllTrueC],'b')
    ylabel(sprintf('x(%d)',ii));
    title(titleCell{ii})
    if(ii==(n-1))
        plot(tvec,[desBearing(1) desBearing],'g--')
    end
end

%perp. speed (added constraint)
subplot(length(plotstates)+1,1,length(plotstates)+1)
stairs(tvec(2:(end-2)),(xAllTrueD(n,2:(N-1))-xAllTrueD(n,1:(N-2))).*CdAll(n,n)./dt,'k')
title('speed perpendicular to trackline')
%xlabel('t [sec]')
ylabel('speed [m/s]')

% subplot(length(plotstates)+2,1,length(plotstates)+2)
% stairs(tvec(2:(end-2)),(uAllMPC(1,2:(N-1))-uAllMPC(1,1:(N-2)))./dt,'k')
% title('change in heading setpoint')
% xlabel('t [sec]')
% ylabel('delta \psi [deg/s]')

figure
% plot cross track error and control
subplot(2,1,1)
stairs(tvec(1:end-1),CdAll(n,:)*[xAllTrueD],'k')
hold on
%stairs(tvec,cdall(4,:)*[xallest(:,:,1) [0 0 0 0]'],'b')
plot(tvec,CdAll(n,:)*[xAllEst],'rx')
plot(tvecc(1:end-1),Cc(n,:)*[xAllTrueC],'b')

set(gca,'fontsize',16);
title({'cross track error',...
    sprintf('t = %d steps, dt = %0.2f sec, mu = %0.1f,  ',T,dt,mu)});
ylabel('cross track error [m]')
legend('discrete','kf estimate','continuous')
subplot(2,1,2);
set(gca,'fontsize',16);

stairs(tvec,[uAllMPC(1,1:(end))],'k');
title('Control: \Delta \Psi')
xlabel('t [sec]');
ylabel('\Delta \psi [deg]');
hold on
plot(tvec(packetLossInds),zeros(length(packetLossInds)),'ro')


%% plot all control plans...

figure
%colors=hsv(n);

%umaxs=umax;
umaxs=max(abs(uAllMPC));
stairs(0:N,[uAllMPC(1,1:N) 0]+2.2*umaxs,'b')
hold on
plot([0 N],2.2*umaxs*ones(1,2),'k--','linewidth',1)
plot([0 N],[0 0],'k')

umaxs=0;
for i=2:N
    umaxs=max([umaxs max(abs(uSave{i}))]);
end

for i=2:N
    if(ifPLossAll(i)==1)
        stairs((i-1):(i-2+T),[uSave{i}] - (i-1)*(2.2*umaxs),'r')
    else
        stairs((i-1):(i-2+T),[uSave{i}] - (i-1)*(2.2*umaxs))
    end
    plot([0 N],-(i-1)*2.2*umaxs*ones(1,2),'k--','linewidth',1)
end
ylim = get(gca,'ylim');
axis([0 N ylim(1) 10*max(abs(uAllMPC))])
set(gca,'yticklabel',[])
title('all control plans, actual is on top')
xlabel(sprintf('time steps (each is %0.2f sec)',dt))


%%
figure
startVec = [19 20 21 22 23];
%startVec = [11 12 13];
%startVec = [2 3 4 5];
%startVec = [8 9 10 11];
%startVec = [35 37 39 41];

colorvec = {'b*--','r*--','g*--','m*--','c*--'};
for j = 1:length(startVec)
    start = startVec(j);
    if((start+T+1)<=N)
        tPart = tvec(start:start+T+1);
        dH = dDesHeading(start-1:start+T);
    else
        tPart = tvec(start):dt:tvec(start)+dt*(T+1);
        dH = [dDesHeading(start-1:N) dDesHeading(N)*ones(1,(T)-(N-start))];
    end
    for i = 1:n
        subplot(n+1,1,i+1)
        stairs(tPart,XMPCSave{start}(i,:),colorvec{j})
        hold on
        ylabel(sprintf('x(%d)',i))
        if(i==n-1)
            plot(tPart,dH,'k:')
        end
    end
    subplot(n+1,1,1)
    stairs(tPart,[0 uSave{start}(1) uSave{start+1}],colorvec{j})
    hold on
    stairs(tPart(1:2),[uSave{start-1}(1) uSave{start-1}(1)],'k')
    title(sprintf('step: %d ',startVec))
    ylabel('u')
end
title({sprintf('step: %d ',startVec);'black: control applied, color: next computed'})