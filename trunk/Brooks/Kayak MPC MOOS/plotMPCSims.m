% plot MPC Sim Results
%

% BR, 8/19/2012

%% print some parameters and results
fprintf('qmpc: \n')
disp(Qmpc)
%fprintf('rmpc: \n')
%disp(Rmpc)
fprintf('pmpc: \n')
disp(Pmpc)
fprintf('slew rate constrained to: %0.1f deg/s\n',slewRate)
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
plotstates=1:(n-1);
for ii = plotstates;
    subplot(length(plotstates)+2,1,ii-min(plotstates)+1);
    set(gca,'fontsize',16);
    stairs(tvec(1:end-1),CdAll(ii,:)*[xAllTrueD],'k');
    hold on
    %stairs(tvec,cdall(ii,:)*[xallest [0;0;0;0]],'b');
    plot(tvec,CdAll(ii,:)*[xAllEst],'rx')
    plot(tvecc(1:end-1),Cc(ii,:)*[xAllTrueC],'b')
    ylabel(sprintf('x(%d)',ii));
    if(ii==3)
        plot(tvec,[desBearing(1) desBearing],'g--')
    end
end

%perp. speed (added constraint)
subplot(length(plotstates)+2,1,length(plotstates)+1)
stairs(tvec(2:(end-2)),(xAllTrueD(n,2:(N-1))-xAllTrueD(n,1:(N-2))).*CdAll(n,n)./dt,'k')
title('speed perpendicular to trackline')
%xlabel('t [sec]')
ylabel('speed [m/s]')

subplot(length(plotstates)+2,1,length(plotstates)+2)
stairs(tvec(2:(end-2)),(uAllMPC(1,2:(N-1))-uAllMPC(1,1:(N-2)))./dt,'k')
title('rudder slew rate')
xlabel('t [sec]')
ylabel('udot [deg/s]')

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
switch syss
    case 'crossTrack'
        title('rudder angle')
    case 'crossTrack_CLheading'
        title('desired heading')
end
xlabel('t [sec]');
ylabel('u1 [deg]');
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
    umaxs=max([umaxs max(uSave{i})]);
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
axis([0 N ylim])
set(gca,'yticklabel',[])
title('all control plans, actual is on top')
xlabel(sprintf('time steps (each is %0.2f sec)',dt))


