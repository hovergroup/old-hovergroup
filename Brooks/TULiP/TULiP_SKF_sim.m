% TULiP simulation using SPF
% this script simulates "true" vehicles (speed saturation) as well as
% packet-loss and quantization effects.
%
% BR, 4/9/2013 (Modifying FSH previous code)

% (make separate script to study just the frequency response of the SPF?)

% Demo a cheap sigma-point filter for range-only tracking in the
% plane.  "Cheap" because it uses sigma points only for the
% target state, but NOT for process noise or for sensor noise.
% Process noise is inserted at random to go with the state
% sigma point evolutions.  The update step uses the regular
% EKF formulation for gain and covariance update, based on
% linearization of the observation, and a single estimated
% measurement.  On the other hand, this filter allows a user-
% specified number of Hermite quadrature points, which may be
% useful.
%
% The range measurements are made from a set of mobile agents,
% which are assumed to follow the estimated target, with some
% transient errors and a delay.
% FSH MIT MechE October 2012

%{
- 4/16/2013: set up one set of parameters to match 11/2012 exp
- 4/25/2013: added a few different quantization settings...
    - uniform (b(1) = 12.5, rho = 1)
    - log from 11/28 (b(1)=3, rho = 0.4775)
    - newer log seems to work ok (using designLogQuantizer)
- 4/26/2013: set up noise sequence in initialization, added options for
    randn with rate limit, or randomized halton seq

%}

clear all;close all;clc

%settings = '11-2012';   % recreate experiment from 11/2012
settings = 'plan';

global localNoise ; % used to pass a ZOH process noise level (Q) into ode45
global targetSpeed;

ifPlot =1;
% interval for plotting "triangle"
plotInt = 10;

dt = 12 ; % time step between samples
dim = 3 ; % dimension of the state space - should match getHermite below
nAgents = 2;

switch settings
    case '11-2012'
        steps = 330 ; % time steps to simulate
        %steps = 100;
        
        probPLossLF = 0.05;
        probPLossFL = 0.09;
        targetSpeed = 1.5;
        R = diag([10 49]);
        Q = .02 ; % target process noise [rad/s]^2
            % target heading rate (held over filter step)
            %   w = sqrt(Q)*randn [rad/s]
        
        % Agent process noise (currents)
        WX = 4;
        WY = 4;
        % Agent dyanamics:
        % [leader, follower];
        maxSpeed = [1.5 2]';
        
        % LOG QUANTIZATION (same settings as 11/2012 exp)
        % b(1) = 3, rho = 0.4775
        b = [27.6 40.7 47 50 53 59.3 72.4];
        binSet = 3;
        
        simQ = Q;
        rateLimit = 1000;
        noiseSeq = sqrt(simQ)*randn(steps,1);

    case 'plan'
        steps = 300 ; % time steps to si mulate
        
        probPLossLF = 0.1;
        probPLossFL = 0.1;
        targetSpeed = 1.5;    % m/s
        %R = diag([10 49]) ; % range sensor noise covariance, per agent
        R = diag([9 25]);
        
        Q = .05 ; % target process noise [rad/s]^2
            % target heading rate (held over filter step)
            %   w = sqrt(Q)*randn [rad/s]        
        
        simQ = 0.01;    % simulated target process noise
        rateLimit = deg2rad(135/dt);
        %{
        noiseSeq = zeros(1,steps);
        for i = 1:steps
            dum = sqrt(simQ)*randn;
            while(abs(dum)>rateLimit)
                dum = sqrt(simQ)*randn;
            end
            noiseSeq(i) = dum ;
        end
        %}
        
        % randomized order halton seq,
        % scaled from -0.5-0.5 to -rateLimit-rateLimit
        noiseSeq = rateLimit*(createHalton(1,steps)-0.5);
        noiseSeq = noiseSeq(randperm(steps));
        
        % Agent process noise (currents)
        WX = 4;
        WY = 4;
        % [leader, follower];
        maxSpeed = [2 2]';
        
        % quantization: bin edges
        % LOG - 7.5 b(1), rho = 0.75...
        b = [19.1667 32.5 42.5 50 57.5 67.5 80.8333];
        binSet = 75;
        % UNIFORM
        %b = [12.5 25 37.5 50 62.5 75 87.5];
        %binSet = 0;
                
end


% set the initial conditions
% Note state is target's: [heading, Cartesian X, Cartesian Y]
xhat = [0 0 0]' ; % guessed
xtrue = [0 50 25]' ; % true
P = diag([5 2500 2500]) ; % state covariance

% formation
legLen = 50;        % meters
theta = deg2rad(60);    % degrees (total angle between each leg)
XAgent0 = [-sin(theta/2)*legLen sin(theta/2)*legLen]'+xhat(2); % sets desired formation
YAgent0 = [-cos(theta/2)*legLen -cos(theta/2)*legLen]'+xhat(3);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

figure(1);clf;hold off;

figure(2);clf;hold off;
plot(xhat(2),xhat(3),'v',...
    xtrue(2),xtrue(3),'k.',...
    'LineWidth',10);

hold on

plot(XAgent0(1),YAgent0(1),'rs','LineWidth',2);
plot(XAgent0(2),YAgent0(2),'gs','LineWidth',2);
plot([XAgent0(1) xhat(2)],[YAgent0(1) xhat(3)],'k');
plot([XAgent0(2) xhat(2)],[YAgent0(2) xhat(3)],'k');

title('Tracking in the Plane:  triangle: xhat     black dot: true');
axis('equal');

options = odeset('RelTol',1e-12,'AbsTol',1e-12);

if dim == 3,
    [s1,s2,s3,w,vol] = getHermite(NaN); % get quadrature points and weights
else
    disp('getHermite is not set up for this dimension -- Stop.');
    break ;
end;

trueTargetSave = zeros(dim,steps);
xhatSave = zeros(dim,steps);
XAgentSave = zeros(nAgents,steps);
YAgentSave = zeros(nAgents,steps);
XADSave = zeros(nAgents,steps);
YADSave = zeros(nAgents,steps);
zSave = zeros(nAgents,steps);

alpha = [1 1]'; % init control packet loss indicator
XAgent = XAgent0;
YAgent = YAgent0;

loopTimes = zeros(1,steps);
for ind = 1:steps
    itStart = tic;
    
    % evolve the true state (note there is no noise as written)

    localNoise = noiseSeq(ind);
    
    
    [time,dum] = ode45('filterDeriv',[0 dt],xtrue,options);
    xtrue = dum(end,:)';
    
    % Compute desired position for observers (based on initial formation)
    XAgentDes = XAgent0 + xhat(2);
    YAgentDes = YAgent0 + xhat(3);
    
    % position the agents for the NEW measurement based on OLD xhat,
    dist = sqrt((XAgentDes-XAgent).^2 + (YAgentDes-YAgent).^2);
    stepX = XAgentDes - XAgent;
    stepY = YAgentDes - YAgent;
    for i = 1:nAgents
        if(dist(i)>(maxSpeed(i)*dt))
            stepX(i) = maxSpeed(i)*dt.*stepX(i)./dist(i);
            stepY(i) = maxSpeed(i)*dt.*stepY(i)./dist(i);
        end
    end
    
    % leader-follower (control command) packet loss
    if(rand<probPLossLF)
        % pLossLF(ind) = 1;
        disp('LEADER-FOLLOWER CNTRL PACKET LOST')
        alpha = [1 0]';
    else
        alpha = [1 1]';
    end
    
    XAgent = XAgent + alpha.*stepX + randn*sqrt(WX);
    YAgent = YAgent + alpha.*stepY + randn*sqrt(WY);
    
    % make the real observation
    z = zeros(nAgents,1);
    for i = 1:nAgents,
        z(i,1) = sqrt((xtrue(2)-XAgent(i))^2 + (xtrue(3)-YAgent(i))^2) ;
    end;
    z = z + sqrt(R)*randn(nAgents,1);
    
    % QUANTIZE FOLLOWER OBSERVATION
    
%     switch quantMethod
%         case 'uniform'
%             
%             a = z(2)-legLen;
%             if( (a<=(d/2)) && (a>(-d/2)) )
%                 z(2) = 0;
%             elseif( mod(a,d/2)==0 )  % round twds -Inf on bin edges
%                 z(2) = d*floor(a/d);
%             else
%                 z(2) = d*round(a/d);
%             end
%             z(2) = a+legLen;
%             
%         case 'logarithmic'
            
            % quantize
            if( (z(2)>0) && (z(2)<b(1)))
                bin = 0;
            elseif( (z(2)>b(1)) && (z(2)<b(2)))
                bin = 1;
            elseif( (z(2)>b(2)) && (z(2)<b(3)))
                bin = 2;
            elseif( (z(2)>b(3)) && (z(2)<b(4)))
                bin = 3;
            elseif( (z(2)>b(4)) && (z(2)<b(5)))
                bin = 4;
            elseif( (z(2)>b(5)) && (z(2)<b(6)))
                bin = 5;
            elseif( (z(2)>b(6)) && (z(2)<b(7)))
                bin = 6;
            elseif( z(2)>b(7) )
                bin = 7;
            end
            
            %select bin based on range...
            z(2) = decodeFollower(bin,binSet);
            
    %end
    
    % simulate packet loss (follower-leader)
    RR = R;
    if(rand<probPLossFL)
        % pLossFL(ind) = 1;
        disp('FOLLOWER-LEADER MEAS PACKET LOST')
        RR(2,2) = 10e10; % Inf meas noise
    end
    
    [xhat,P] = filterStep(xhat,P,z,XAgent,YAgent,...
        dim,s1,s2,s3,w,vol,Q,dt,RR);
    
    if(ifPlot)
        figure(1);
        h = plot(ind,abs(rem(xhat(1)-xtrue(1)-pi,2*pi)+pi),'.',...
            ind,abs(xhat(2)-xtrue(2)),'.',...
            ind,abs(xhat(3)-xtrue(3)),'.',...
            'LineWidth',2);
        for ii = 1:length(h)
            set(h(ii),'MarkerSize',10)
        end
        title('Error signals:   blue:hdg(rad)    green:X(m)    red:Y(m)');
        xlabel('time index');
        hold on;
        
        figure(2);
        
        plot(xhat(2),xhat(3),'v',...
            xtrue(2),xtrue(3),'k.',...
            'LineWidth',2,'MarkerSize',6);
        hold on
        plot(dum(:,2),dum(:,3),'b')
        %plot(XAgent,YAgent,'r.','LineWidth',2);
        plot(XAgentDes(1),YAgentDes(1),'r.','MarkerSize',4);
        plot(XAgentDes(2),YAgentDes(2),'g.','MarkerSize',4);
        plot(XAgent(1),YAgent(1),'rs','MarkerSize',3)
        plot(XAgent(2),YAgent(2),'gs','MarkerSize',3)
        
        if(ind>1)
            plot([XAgentSave(1,ind-1) XAgent(1)],[YAgentSave(1,ind-1),YAgent(1)],'r','LineWidth',1)
            plot([XAgentSave(2,ind-1) XAgent(2)],[YAgentSave(2,ind-1),YAgent(2)],'g','LineWidth',1)
        end
        
        if(~(mod(ind,plotInt)))
            plot([XAgent(1) xhat(2)],[YAgent(1) xhat(3)],'k','LineWidth',2);
            plot([XAgent(2) xhat(2)],[YAgent(2) xhat(3)],'k','LineWidth',2);
        end
    end
    
    % save variables
    trueTargetSave(:,ind) = xtrue;
    xhatSave(:,ind) = xhat;
    zSave(:,ind) = z;
    XAgentSave(:,ind) = XAgent;
    YAgentSave(:,ind) = YAgent;
    XADSave(:,ind) = XAgentDes;
    YADSave(:,ind) = YAgentDes;
    
    if norm(xhat) > 1e6,
        disp('xhat appears unstable -- Stop.');
        break ;
    end;
    loopTimes(ind) = toc(itStart);
    
end;


%%

% compare TRUE FORMATION (true target), X, Y DESIRED, and actual...

figure
plot(XADSave',YADSave','--')
hold on
plot(XAgentSave',YAgentSave')

%% TF analysis...


% compute ex, ey
ex = xhatSave(2,:) - trueTargetSave(2,:);
ey = xhatSave(3,:) - trueTargetSave(3,:);

% TF analysis
fs = 1/dt;  % Hz
NFFT = 2^nextpow2(length(ex));
fprintf('\n Empirical FFT: NFFT = %d \n',NFFT)
f = fs/2*linspace(0,1,NFFT/2+1);

EX = fft(ex,NFFT);
X = fft(trueTargetSave(2,:),NFFT);
EY = fft(ey,NFFT);
Y = fft(trueTargetSave(3,:),NFFT);

SX = EX./X;
SY = EY./Y;

SY = smooth(SY,10);


w = f*2*pi;

figure

subplot(3,1,2)
semilogx(w, 20*log10(2*abs(SX(1:NFFT/2+1))) ,'b')
hold on
semilogx(w, 20*log10(2*abs(SY(1:NFFT/2+1))) ,'r')
%xlabel('Freq [Hz]')
xlabel('Freq [rad/s]')
ylabel('dB')
yLim = get(gca,'YLim');
xLim = get(gca,'XLim');
plot(xLim,[0 0],'k')
plot([.05 .05],yLim,'c')
grid on
legend('x','y')

subplot(3,1,3)
plot(w, 20*log10(2*abs(SX(1:NFFT/2+1))) ,'b')
hold on
plot(w, 20*log10(2*abs(SY(1:NFFT/2+1))) ,'r')
%xlabel('Freq [Hz]')
xlabel('Freq [rad/s]')
ylabel('dB')
yLim = get(gca,'YLim');
xLim = get(gca,'XLim');
plot(xLim,[0 0],'k')
plot([.05 .05],yLim,'c')
grid on
legend('x','y')


dx = (trueTargetSave(2,2:end) - trueTargetSave(2,1:(end-1)))/dt;
dy = (trueTargetSave(3,2:end) - trueTargetSave(3,1:(end-1)))/dt;
DX = smooth(fft(dx,NFFT),10);
DY = smooth(fft(dy,NFFT),10);


subplot(3,1,1)
semilogx(w,20*log10(2*abs(DX(1:NFFT/2+1))),'b')
hold on
semilogx(w,20*log10(2*abs(DY(1:NFFT/2+1))),'r')
xlabel('Freq [rad/s]')
ylabel('dB')
yLim = get(gca,'YLim');
xLim = get(gca,'XLim');
plot(xLim,[0 0],'k')
plot([.05 .05],yLim,'c')
grid on
legend('x','y')



