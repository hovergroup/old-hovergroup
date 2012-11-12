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
%

% FSH MIT MechE October 2012

clear all;close all;clc

global localNoise ; % used to pass a ZOH process noise level (Q) into ode45
global targetSpeed;

ifPlot = 1;
% interval for plotting "triangle"
plotInt = 10;

probPLossLF = 0.3;
probPLossFL = 0.3;

% problem parameters
targetSpeed = 1;    % m/s
dt = 6 ; % time step between samples
dim = 3 ; % dimension of the state space - should match getHermite below
steps = 100 ; % time steps to simulate
Q = .02 ; % target process noise (heading rate of target)

% Agent process noise (current)
WX = 0.5^2*sqrt(dt);
WY = 0.5^2*sqrt(dt);

% Agent dyanamics:
alpha = [1 1]';
maxSpeed = [1 2]';
nAgents = 2;

%R = diag([25 25]) ; % range sensor noise covariance, per agent
R = diag([4 36]);

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
XAgentSave = zeros(nAgents,steps);
YAgentSave = zeros(nAgents,steps);
XADSave = zeros(nAgents,steps);
YADSave = zeros(nAgents,steps);
xhatSave = zeros(dim,steps);

XAgent = XAgent0;
YAgent = YAgent0;

loopTimes = zeros(1,steps);
for ind = 1:steps
    itStart = tic;
    
    % evolve the true state (note there is no noise as written)
    localNoise = sqrt(Q)*randn ;
    [time,dum] = ode45('filterDeriv',[0 dt],xtrue,options);
    xtrue = dum(end,:)';
    
    % Compute desired position for observers (based on initial formation)
    % (could also hardcode formation here)
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
            'LineWidth',2,'MarkerSize',10);
        hold on
        %plot(XAgent,YAgent,'r.','LineWidth',2);
        plot(XAgentDes(1),YAgentDes(1),'r.','MarkerSize',10);
        plot(XAgentDes(2),YAgentDes(2),'g.','MarkerSize',10);
        plot(XAgent(1),YAgent(1),'rs',XAgent(2),YAgent(2),'gs')
        
        if(ind>1)
            plot([XAgentSave(1,ind-1) XAgent(1)],[YAgentSave(1,ind-1),YAgent(1)],'r--','LineWidth',1)
            plot([XAgentSave(2,ind-1) XAgent(2)],[YAgentSave(2,ind-1),YAgent(2)],'g--','LineWidth',1)
        end
        
        if(~(mod(ind,plotInt)))
            plot([XAgent(1) xhat(2)],[YAgent(1) xhat(3)],'k--','LineWidth',1);
            plot([XAgent(2) xhat(2)],[YAgent(2) xhat(3)],'k--','LineWidth',1);
        end
    end
    
    trueTargetSave(:,ind) = xtrue;
    XAgentSave(:,ind) = XAgent;
    YAgentSave(:,ind) = YAgent;
    XADSave(:,ind) = XAgentDes;
    YADSave(:,ind) = YAgentDes;
    
    xhatSave(:,ind) = xhat;
    
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
