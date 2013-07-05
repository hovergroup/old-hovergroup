% MOOS Kayak Ranging Tracker
% 2 kayaks tracking a (virtual) target via range measurements
% target position relative to baseline assumed known
% kayaks attempt to stay in formation (triangle, length 100m)
% this script: target estimator with modified UKF, plus iMatlab interface

% BR, 10/12/2012

% changelog:
%{
- added targetSpeed, added MOOS logging of estimate
- 4/30/2013: updated parameter inits, added rateLimit option for SPKF,
    added binSet option to decodeFollower
- 7/3/2013: added options for full packets, ideal comms

%}

clear all;
clear iMatlab; clc
global targetSpeed

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% USER SETTINGS
PRINTOUTS = 1;  % to matlab terminal
diaryName = sprintf('TULiP_diary_%s.txt',dateString('DHM'));
% (note: add process config to meta_shoreside.moos)
moosDB = 'targ_shoreside.moos';
%pathName = '/home/josh/hovergroup/ivp-extend/josh/missions/121119_TargetTulip/';
pathName = '/home/josh/hovergroup/ivp-extend/missions/josh/TulipTarget';

experiment = 'mini';
%experiment = 'full';
%experiment = 'asym';
%experiment = 'ideal';

% time step
switch experiment
    case 'mini'
        dt = 12;
        
        varList = {'FOLLOWER_X','FOLLOWER_Y','FOLLOWER_RANGE_BIN',...
            'FOLLOWER_PACKET','LEADER_X','LEADER_Y','LEADER_RANGE'};
        
        % QUANTIZATION
        % 11/2012: binSet 3
        % 4/2013: binSet 0
        % 6/2013: binSet 75
        
        % b(1) = 3, rho = 0.4775
        % bin edges: [27.6 40.7 47 50 53 59.3 72.4];
        % binSet = 3;
        
        % LOG - v2
        % b(1) = 7.5, rho = 0.75
        % bin edges: [19.1667 32.5 42.5 50 57.5 67.5 80.8333];
        binSet = 75;
        
        % uniform
        % b(1) = 12.5, rho = 1
        % bin edges: [12.5 25 37.5 50 62.5 75 87.5];
        %binSet = 0;
        
        % range sensor noise covariance, per agent; z(1) = leader, z(2) = follower
        Rmeas = diag([1 9]);
        % 11/2012: diag([9 49])
        % 4/2013: diag([4 9]);
        % 6/2013: diag([1 9]);
        
    case 'full'
        % (follower x,y, range should not be quantized now)
        % still have to handle missed packets
        
        dt = 23;
        
        % (FOLLOWER_RANGE vs. FOLLOWER_RANGE_BIN: no decode)
        varList = {'FOLLOWER_X','FOLLOWER_Y','FOLLOWER_RANGE',...
            'FOLLOWER_PACKET','LEADER_X','LEADER_Y','LEADER_RANGE'};
        
        % range sensor noise covariance, per agent; z(1) = leader, z(2) = follower
        Rmeas = diag([.25 .25]);
        
    case 'asym'
        % (follower x,y, range should not be quantized now)
        % still have to handle missed packets
        
        dt = 17;
        
        % (FOLLOWER_RANGE vs. FOLLOWER_RANGE_BIN: no decode)
        varList = {'FOLLOWER_X','FOLLOWER_Y','FOLLOWER_RANGE',...
            'FOLLOWER_PACKET','LEADER_X','LEADER_Y','LEADER_RANGE'};
        % range sensor noise covariance, per agent; z(1) = leader, z(2) = follower
        Rmeas = diag([.25 .25]);
        
    case 'ideal'
        
        dt = 4;
        
        % (FOLLOWER_RANGE vs. FOLLOWER_RANGE_BIN: no decode)
        varList = {'FOLLOWER_X','FOLLOWER_Y','FOLLOWER_RANGE',...
            'FOLLOWER_PACKET','LEADER_X','LEADER_Y','LEADER_RANGE'};
        
        % range sensor noise covariance, per agent; z(1) = leader, z(2) = follower
        Rmeas = diag([.25 .25]);
        
end


% ESTIMATOR PARAMETERS
% target speed: (match motorboat (and kayak) speed)
targetSpeed = 1.55;    % m/s

% target process noise (heading rate of target) [rad/s]^2
Q = .05 ;
% 11/2012: 0.02
% 4/2013: 0.05
% 6/2013: 0.05

% inside SPKF, noise drawn according to sqrt(Q)*randn subj to rateLimit
rateLimit = deg2rad(120/dt);
% 11/2012: Inf
% 4/2013: 135
% 6/2013: 120


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

diary(diaryName)
disp(experiment)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% initial covariance and state
% [heading metersE metersN] from origin (pavilion)
xhat=[0 20 -20]';
P = diag([5 2500 2500]) ;
z = zeros(2,1); % preallocate

dim = 3 ; % dimension of the state space - should match getHermite below
nAgents = 2;

% formation
legLen = 50;        % meters
theta = deg2rad(60);    % degrees (total angle between each leg)
XAgent0 = [-sin(theta/2)*legLen sin(theta/2)*legLen]'; % sets desired formation
YAgent0 = [-cos(theta/2)*legLen -cos(theta/2)*legLen]';
XAgent = XAgent0;
YAgent = YAgent0;

if dim == 3,
    [s1,s2,s3,w,vol] = getHermite(NaN); % get quadrature points and weights
else
    disp('getHermite is not set up for this dimension -- Stop.');
    break ;
end;

% INITIALIZE IMATLAB:
old = cd(pathName);
% subscribe
iMatlab('init','CONFIG_FILE',moosDB);
garbageMail = iMatlab('MOOS_MAIL_RX');
%length(garbageMail)
cd(old);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
it = 0;
go = 1; % possibly add exitflags to loop?
while(go)
    
    it = it+1;
    fprintf('\n m IT: %d \n',it)
    itStart = tic;
    
    % GRAB OBSERVATIONS
    % Leader: x,y,range
    % Follower: x,y,range
    
    readTimeout = dt+3;
    data = parseObservations(varList,readTimeout);
    
    if(strcmp(data.status,'timeout'))
        disp('data read timeout!')
        % do nothing
    else % good data
        
        % measurements used by all exps
        XAgent(1) = data.LEADER_X;
        YAgent(1) = data.LEADER_Y;
        
        % possible missed leader ranges
        z(1) = data.LEADER_RANGE;
        R = Rmeas;
        if(z(1)==-1)
            z(1) = 0;R(1,1) = 10e10;
            disp('MISSING LEADER RANGE')
        end
        
        % follower packet drops (still posted for wifi)
        if(data.FOLLOWER_PACKET==0)
            disp('MISSING FOLLOWER PACKET')
            z(2) = 0;R(2,2) = 10e10; % Inf meas noise
        else    % good follower packet
            XAgent(2) = data.FOLLOWER_X;
            YAgent(2) = data.FOLLOWER_Y;
            
            % ranges (mini is quantized)
            if(strcmp(experiment,'mini'))
                % decode packet
                if(data.FOLLOWER_RANGE_BIN==-1)
                    % follower missed it's range... data useless
                    disp('MISSING FOLLOWER RANGE')
                    z(2) = 0;R(2,2) = 10e10;
                else
                    z(2) = decodeFollower(data.FOLLOWER_RANGE_BIN,binSet);
                end
            else    % ideal, full, asym: range not quantized
                % grab range
                if(data.FOLLOWER_RANGE==-1)
                    % follower missed it's range... data useless
                    disp('MISSING FOLLOWER RANGE')
                    z(2) = 0;R(2,2) = 10e10;
                else
                    z(2) = data.FOLLOWER_RANGE;
                end
            end
            
        end
        
        % run SPKF
        [xhat,P] = filterStep(xhat,P,z,XAgent,YAgent,...
            dim,s1,s2,s3,w,vol,Q,dt,R,rateLimit);
        
        if norm(xhat) > 1e6,
            disp('xhat appears unstable -- Stop.');
            break ;
        end;
        
        % post estimate to DB
        iMatlab('MOOS_MAIL_TX','TARGET_EST_H',xhat(1));
        iMatlab('MOOS_MAIL_TX','TARGET_EST_X',xhat(2));
        iMatlab('MOOS_MAIL_TX','TARGET_EST_Y',xhat(3));
        
        % post estimate to pMarineViewer
        view_marker = sprintf(...
            'type=square,x=%f,y=%f,label=estimate,COLOR=blue,msg=Est: %f %f', ...
            xhat(2), xhat(3), xhat(2), xhat(3) );
        iMatlab('MOOS_MAIL_TX','VIEW_MARKER',view_marker);
        
        % Compute desired position for observers (based on initial formation)
        % (could also hardcode formation here)
        XAgentDes = XAgent0 + xhat(2);
        YAgentDes = YAgent0 + xhat(3);
        
        lwps = sprintf('%f,%f',XAgentDes(1),YAgentDes(1));
        fwps = sprintf('%f,%f',XAgentDes(2),YAgentDes(2));
        iMatlab('MOOS_MAIL_TX','LEADER_WAYPOINT_SILVANA',lwps);
        iMatlab('MOOS_MAIL_TX','FOLLOWER_WAYPOINT_SILVANA',fwps);
        
        if(PRINTOUTS)
            
            % post measurements
            fprintf('LX: %f  LY: %f  FX: %f  FY: %f  \n',data.LEADER_X,...
                data.LEADER_Y, data.FOLLOWER_X, data.FOLLOWER_Y);
            fprintf('LR: %f  FR:  %f \n',data.LEADER_RANGE, z(2));
            if(strcmp(experiment,'mini'))
                fprintf('F bin: %d \n',data.FOLLOWER_RANGE_BIN);
            end
            
            % post estimate
            hrad = xhat(1);
            while(abs(hrad)>2*pi)
                if(hrad>2*pi)
                    hrad = hrad-2*pi;
                elseif(hrad<(-2*pi))
                    hrad = hrad+2*pi;
                end
            end
            bearing = 90 - rad2deg(hrad);
            fprintf('\n Target X: %f   Target Y: %f   Target Bearing: %f \n',...
                xhat(2),xhat(3),bearing)
            
            % post command
            fprintf(['leader non-quantized wpt: ' lwps '\n'])
            fprintf(['follower non-quantized wpt: ' fwps '\n'])
            
        end
        
    end
    
    mloopTime = toc(itStart);
    fprintf('matlab time: %f \n',mloopTime)
    
end

%%%% TURN DIARY OFF %%%%%
diary off


% NOTES ON MODIFIED SPKF from Franz

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
