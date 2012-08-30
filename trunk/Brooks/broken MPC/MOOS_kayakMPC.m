% Model Predictive Control for cross-track error
% Hovergroup kayaks, using MOOS, iMatlab
% Solved with CVX

% BR, 8/13/2012

% changes
%{
- 8/14/2012: added set point stuff (cut out heading error wrap for now)
- 8/14/2012: some minor changes while testing
- 8/17/2012: added increment to N, adjusted xDes increments (still a bit
off though).
- 8/19/2012: added initializeMOOS_MPC script
    - fixed some indexing issues with desBearing
    - added computeMPCInputs fcn
- 8/20/2012: added options for different systems (still need to adjust
    parseMPC_XEST)
- 8/21/2012: added a few mods for constant heading behavior update


%}
clear all
close all
clc
format compact

diary on


TX = 'wifi';
%TX = 'acomms';

% configure MPC parameters
configureKayakMPC;

% init
xDes = zeros(n,T+2);eDes = zeros(n,T+2);uPlan = zeros(m,T);
eEst = zeros(n,1); % mei's KF hardcoded for 4 states...
eEstKF = zeros(4,1);
xHat = zeros(n,1);
kPlan = 1;  % no init packet loss
uPlanBuffered = uPlan;

% 5 + T*(lenU) + len_tMPC + len_ifPLoss
lenMPC_STR=5+T*10+19+14;
MPC_STR = char(97*ones(1,lenMPC_STR));

initializeMOOS_MPC;

mpcStart = tic;
loopIt=1;
% start loop  (breaks when MPC_STOP==1)
while(~mpc_stop)
    
    loopStart = tic;
    runStep = floor(toc(mpcStart)/dt)+1;
    fprintf('Step: %i, loopIt: %i\n\n',runStep,loopIt)
    
    % grab new estimate (after first loop)
    if(loopIt>1)
    %if(1)
        % cross-track and heading ERROR relative to desBearing(step)
        [eEstKF mpc_stop] = parseMPC_XEST;
        xHat = eEstKF + [0 0 desBearing(loopIt-1) 0]';
        fprintf('actual est heading: %f [deg]\n',xHat(3))
        fprintf('cross-track error: %f [m]\n\n',CdAll(n,n)*eEstKF(4))
    end
    switch syss
        case'crossTrack_CLheading'
            eEst = eEstKF(2:4);
        case 'crossTrack'
            eEst = eEstKF;
    end
    
    % create inputs to MPC: eDes, uPrev:
    eDes = computeMPCInputs(n,N,T,syss,desBearing,loopIt);
    disp(eDes(n-1,:))
    
    if(loss2MPC)
        % MPC knows previous command is the buffered value
        uPrev = u;
    else
        % MPC assumes previous command was the one it computed
        uPrev = uPlan(:,1);
    end
    % solve MPC - xEst and previous control are inputs
    [uPlan tMPC] = solveKayakMPC(sys,eEst,MPCparams,uDelay,uPrev,eDes);
    
    % SIM PACKET LOSS (WITH DELAY)
    % buffer works on RECEIVED PLAN AT THIS TIME STEP (delayed)
    [uPlanBuffered kPlan ifPLoss] = simPacketLossMPC...
        (uPlan,uPlanBuffered,kPlan,probPLoss);
    u = uPlanBuffered(:,kPlan);     % send appropriate control action
    %u = uPlan(:,1);
    
    fprintf('Computed plan: \n')
    disp(uPlan)
    fprintf('Buffered plan: \n')
    disp(uPlanBuffered)
    
    % (encode, quantize plan)
    % send plan (just control to start)
    
    switch syss
        case 'crossTrack'
            send = u;  %+rOff; (rOff in MOOS)
        case 'crossTrack_CLheading'
            uBearing = u+desBearing(loopIt);
            if(uBearing<0);uBearing = uBearing+360;end
            if(uBearing>360);uBearing = uBearing - 360;end
            send = sprintf('heading = %0.1f',uBearing);
    end
    
    switch TX
        case 'wifi'
            
            % DELAY (simulate acomms...)
            while(toc(loopStart)<(dt-.005))
                pause(0.005)
            end
            switch syss
                case 'crossTrack'
                    iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',send);
                    fprintf('\n\nSend u = %f, Send rudder = %f \n\n\n',u,send);
                case 'crossTrack_CLheading'
                    iMatlab('MOOS_MAIL_TX','CONST_HEADING_UPDATES',send);
                    fprintf('\n\nSend des heading = %f \n\n\n',uBearing);
            end
            
        case 'acomms'
            
            
    end
    
    % string to log
    MPC_STR(1:5) = 'uPlan';
    lenU = 7+3; % u field plus delimiter
    for k = 1:T
        MPC_STR((6+((k-1)*lenU)):(5+(k*lenU))) = sprintf('<|>%+07.3f',uPlan(k));
        %[MPC_STR ',' num2str(uPlan(k))];
    end
    MPC_STR(6+(k*lenU):(6+(k*lenU)-1)+19) = sprintf('<|>tMPC<|>%09.3f',tMPC);
    MPC_STR((6+(k*lenU))+19:lenMPC_STR) = sprintf('<|>ifPLoss<|>%d',ifPLoss);
    
    
    % send a big string of a bunch of other stuff:
    iMatlab('MOOS_MAIL_TX','MPC_STR',MPC_STR)
    
    % check if at end
    if(loopIt>=(N-1+leg1Steps))
        mpc_stop=1;
        disp('FINISHED TRACKLINE LIST')
    end
    loopIt = loopIt+1;
    
    loopTime = toc(loopStart);
    fprintf('\nLoop Time: %f sec\n',loopTime)
    
end

diary off

cd(old)


