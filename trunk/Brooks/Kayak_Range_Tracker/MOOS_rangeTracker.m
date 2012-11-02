% MOOS Kayak Ranging Tracker
% 2 kayaks tracking a (virtual) target via range measurements
% target position relative to baseline assumed known
% kayaks attempt to stay in formation (equilateral triangle, length 100m)
% this script: target estimator with modified UKF, plus iMatlab interface

% BR, 10/12/2012

% changelog:
%{
-
-

%}

%global localNoise ; % used to pass a ZOH process noise level (Q) into ode45


% problem parameters
dt = 9 ; % time step between samples
dim = 3 ; % dimension of the state space - should match getHermite below
nAgents = 2;

% estimator parameters
% Note state is target's: [heading, Cartesian X, Cartesian Y]
Q = .02 ; % target process noise (heading rate of target)
            % PSD: (deg/s)^2 / Hz?  

Rmeas = diag([25 25]) ;     % range sensor noise covariance, per agent

P = diag([5 2500 2500]) ;   % INITIAL state covariance
xhat = [0 0 0]' ; % initial guess

% formation 
legLen = 50;        % meters
theta = deg2rad(60);    % degrees (total angle between each leg)
XAgent0 = [-sin(theta/2)*legLen sin(theta/2)*legLen]'+xhat(2); % sets desired formation
YAgent0 = [-cos(theta/2)*legLen -cos(theta/2)*legLen]'+xhat(3);

%options = odeset('RelTol',1e-12,'AbsTol',1e-12);

if dim == 3,
    [s1,s2,s3,w,vol] = getHermite(NaN); % get quadrature points and weights
else
    disp('getHermite is not set up for this dimension -- Stop.');
    break ;
end;

gotStart = 0;
while(gotStart)
    
    startData = parseObservations();
    
    if(startData.FOLLOWER_PACKET==0)
        
        disp('MISSING FOLLOWER PACKET')
        continue
    
    else
        
        XAgent(1) = startData.LEADER_X;
        XAgent(2) = startData.FOLLOWER_X;
        YAgent(1) = startData.LEADER_Y;
        YAgent(2) = startData.FOLLOWER_Y;
        gotStart = 1;
        
    end
    
end

go = 1; % possibly add exitflags to loop?
while(go)
    
    itStart = tic;
    
    % GRAB OBSERVATIONS
    % Leader: x,y,range
    % Follower: x,y,range
    
    % set readTimeout short and only call after delay at end of loop?
    % or readTimeout long? 
    readTimeout = 7;
    data = parseObservations(readTimeout);
    
    if(data.FOLLOWER_PACKET==0)
        disp('MISSING FOLLOWER PACKET')
        
        XAgent(1) = data.LEADER_X;
        YAgent(1) = data.LEADER_Y;
        z(1) = data.LEADER_R;
        
        % Set follower meas noise to INF
        R = Rmeas;
        R(2,2) = 10e10; % Inf meas noise
        % USE OLD FOLLOWER INFO...no changes

    else
        
        XAgent(1) = data.LEADER_X;
        XAgent(2) = data.FOLLOWER_X;
        YAgent(1) = data.LEADER_Y;
        YAgent(2) = data.FOLLOWER_Y;
        z(1) = data.LEADER_R;
        z(2) = decodeFollower(data.FOLLOWER_RANGE_BIN);
        
        R = Rmeas;
        
    end
    
    [xhat,P] = filterStep(xhat,P,z,XAgent,YAgent,...
        dim,s1,s2,s3,w,vol,Q,dt,R);
    
    if norm(xhat) > 1e6,
        disp('xhat appears unstable -- Stop.');
        break ;
    end;
    
    % Compute desired position for observers (based on initial formation)
    % (could also hardcode formation here)
    XAgentDes = XAgent0 + xhat(2);
    YAgentDes = YAgent0 + xhat(3);
    
    % send WAYPOINT_UPDATES back to MOOS
    lwps = sprintf('points=%f,%f',XAgentDes(1),YAgentDes(1));
    fwps = sprintf('points=%f,%f',XAgentDes(2),YAgentDes(2));
    iMatlab('MOOS_MAIL_TX','LEADER_WAYPOINT_UPDATES',lwps);
    iMatlab('MOOST_MAIL_TX','FOLLOWER_WAYPOINT_UPDATES',fwps);
    
    
    mloopTime = toc(itStart);

end
