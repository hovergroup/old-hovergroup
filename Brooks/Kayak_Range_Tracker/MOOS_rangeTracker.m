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

clear iMatlab; clc

PRINTOUTS = 1;

% problem parameters
dt = 6 ; % time step between samples

dim = 3 ; % dimension of the state space - should match getHermite below
nAgents = 2;

% estimator parameters
% Note state is target's: [heading, Cartesian X, Cartesian Y]
Q = .02 ; % target process noise (heading rate of target)
% PSD: (deg/s)^2 / Hz?

Rmeas = diag([25 25]) ;     % range sensor noise covariance, per agent

P = diag([5 2500 2500]) ;   % INITIAL state covariance
%xhat = [0 0 0]' ; % initial guess
xhat = [0 0 -30]';	% [heading metersN metersS] from origin (pavilion)
z = zeros(2,1);

% formation
legLen = 50;        % meters
theta = deg2rad(60);    % degrees (total angle between each leg)
XAgent0 = [-sin(theta/2)*legLen sin(theta/2)*legLen]'+xhat(2); % sets desired formation
YAgent0 = [-cos(theta/2)*legLen -cos(theta/2)*legLen]'+xhat(3);

if dim == 3,
    [s1,s2,s3,w,vol] = getHermite(NaN); % get quadrature points and weights
else
    disp('getHermite is not set up for this dimension -- Stop.');
    break ;
end;

% INITIALIZE IMATLAB:
% (note: add process config to meta_shoreside.moos)
moosDB = 'targ_shoreside.moos';
pathName = '/home/josh/hovergroup/ivp-extend/josh/missions/121015_26bit_tulip/';
old = cd(pathName);

% subscribe 
iMatlab('init','CONFIG_FILE',moosDB);
garbageMail = iMatlab('MOOS_MAIL_RX');
%length(garbageMail)

it = 0;
go = 1; % possibly add exitflags to loop?
while(go)
    
    it = it+1;
    fprintf('\n m IT: %d \n',it)
    
    itStart = tic;
    
    % GRAB OBSERVATIONS
    % Leader: x,y,range
    % Follower: x,y,range
    
    readTimeout = 10;
    data = parseObservations(readTimeout)
    
    % CHECK data.status??
    
    if(data.FOLLOWER_PACKET==0)
        disp('MISSING FOLLOWER PACKET')
        
        XAgent(1) = data.LEADER_X;
        YAgent(1) = data.LEADER_Y;
        z(1) = data.LEADER_RANGE;
        
        % Set follower meas noise to INF
        R = Rmeas;
        R(2,2) = 10e10; % Inf meas noise
        % USE OLD FOLLOWER INFO...no changes
        
    else
        
        XAgent(1) = data.LEADER_X;
        XAgent(2) = data.FOLLOWER_X;
        YAgent(1) = data.LEADER_Y;
        YAgent(2) = data.FOLLOWER_Y;
        z(1) = data.LEADER_RANGE;
        z(2) = decodeFollower(data.FOLLOWER_RANGE_BIN);
        
        R = Rmeas;
        
    end
    
    if(PRINTOUTS)
    fprintf('LX: %f  LY: %f  FX: %f  FY: %f  \n',data.LEADER_X,...
        data.LEADER_Y, data.FOLLOWER_X, data.FOLLOWER_Y);
    fprintf('F bin: %d \n',data.FOLLOWER_RANGE_BIN);
    fprintf('LR: %f  FR:  %f \n',data.LEADER_RANGE, z(2));
    end
    
    [xhat,P] = filterStep(xhat,P,z,XAgent,YAgent,...
        dim,s1,s2,s3,w,vol,Q,dt,R);
    
    if norm(xhat) > 1e6,
        disp('xhat appears unstable -- Stop.');
        break ;
    end;
    
    if(PRINTOUTS)
        fprintf('\n Target X: %f   Target Y: %f   Target H: %f \n',xhat(2),xhat(3),xhat(1))
    end

    % post estimate to DB
    iMatlab('MOOS_MAIL_TX','TARGET_EST_H',xhat(1));
    iMatlab('MOOS_MAIL_TX','TARGET_EST_X',xhat(2));
    iMatlab('MOOS_MAIL_TX','TARGET_EST_Y',xhat(3));
    
    % post estimate to pMarineViewer
    view_marker = sprintf('type=square,x=%f,y=%f,label=estimate,COLOR=blue,msg=Est: %f %f', ...
        xhat(2), xhat(3), xhat(2), xhat(3) );
    iMatlab('MOOS_MAIL_TX','VIEW_MARKER',view_marker);	
    
    % Compute desired position for observers (based on initial formation)
    % (could also hardcode formation here)
    XAgentDes = XAgent0 + xhat(2);
    YAgentDes = YAgent0 + xhat(3); 
    
    lwps = sprintf('%f,%f',XAgentDes(1),YAgentDes(1));
    fwps = sprintf('%f,%f',XAgentDes(2),YAgentDes(2));
    iMatlab('MOOS_MAIL_TX','LEADER_WAYPOINT',lwps);
    iMatlab('MOOS_MAIL_TX','FOLLOWER_WAYPOINT',fwps);
    
    if(PRINTOUTS)
    fprintf(['leader wpt: ' lwps '\n'])
    fprintf(['follower wpt: ' fwps '\n'])
    end
    
    mloopTime = toc(itStart);
    
    fprintf('matlab time: %f \n',mloopTime)
    
end
