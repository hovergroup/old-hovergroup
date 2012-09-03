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
- 9/1/2012: changed for options of crossTrack or crossTrack_integrator
    - moved some stuff to initializeMOOS_MPC
    - changed computeMPCInputs to loopIt - 1 (uBearing still with loopIt)

%}
clear all
close all
clc
format compact

%% ICs
r0=0;       % if cross-track... initial rudder
hd0=80;    % if heading setpt... initial heading 
% hd0 SHOULD MATCH STARTING CONSTANT HEADING BEHAVIOR SETPT

%% Loop
configureKayakMPC;
initializeMOOS_MPC;
mpcStart = tic;
loopIt=1;

diary on
% start loop  (breaks when MPC_STOP==1)
while(~mpc_stop)
    
    loopStart = tic;
    runStep = floor(toc(mpcStart)/dt)+1;
    fprintf('Step: %i, loopIt: %i\n\n',runStep,loopIt-1)
    
    % grab new estimate (after first loop)
    if(loopIt>1)
        % cross-track and heading ERROR relative to desBearing(step)
        [eEstKF mpc_stop] = parseMPC_XEST(n,sys);
        switch syss
            % xDes is for KF states (not PSI)
            case 'crossTrack'
                xDes = [0 0 desBearing(loopIt-1) 0]';
            case 'crossTrack_integrator'
                xDes = [0 0 0 desBearing(loopIt-1) 0]';
        end
        xHat = eEstKF + xDes;
        fprintf('actual est heading: %f [deg]\n',xHat(n-1))

        % create inputs to MPC: eDes, uPrev:
        eDes = computeMPCInputs(n,N,T,desBearing,loopIt-1);
        disp(eDes(n-1,:))
        
    end
    
    % first state is always ePSI - propagated here
    eEst(1) = eEst(1) + u;
    switch syss
        case 'crossTrack'
            % next states are ehdot, eh, ex
            eEst(2:4) = eEstKF(3:5);
        case 'crossTrack_integrator'
            % next states are intx, ehdot, eh, ex
            eEst(2) = eEstKF(1);
            % (skip state 2: ehddot)
            eEst(3:5) = eEstKF(3:5);
    end
    
    % solve MPC - xEst and previous control are inputs
    uPrev = uPlan(:,1);
    [uPlan tMPC] = solveKayakMPC(sys,eEst,MPCparams,uDelay,uPrev,eDes);
    
    % SIM PACKET LOSS (WITH DELAY)
    % buffer works on RECEIVED PLAN AT THIS TIME STEP (delayed)
    [uPlanBuffered kPlan ifPLoss] = simPacketLossMPC...
        (uPlan,uPlanBuffered,kPlan,probPLoss);
    u = uPlanBuffered(:,kPlan);     % send appropriate control action
    %u = uPlan(:,1);
    
    fprintf('Computed plan: \n')
    disp(uPlan)
    %fprintf('Buffered plan: \n')
    %disp(uPlanBuffered)
    
    %%%% ACTUAL CONTROL TO SEND IS HEADING SETPOINT
    uBearing = eEst(1) + desBearing(loopIt) + u;
    if(uBearing<0);uBearing = uBearing+360;end
    if(uBearing>360);uBearing = uBearing - 360;end
    fprintf('\n Next step heading controller setpoint: %f\n\n',uBearing)
  
    
    % (encode, quantize plan)
    % send plan (just control to start)
    
    
    switch TX
        case 'wifi'
            
            % DELAY (simulate acomms...)
            while(toc(loopStart)<(dt-.005))
                pause(0.005)
            end
            send = sprintf('heading = %0.1f',uBearing);
            iMatlab('MOOS_MAIL_TX','CONST_HEADING_UPDATES',send);
            fprintf('\n\nSend des heading = %f \n\n\n',uBearing);
            
            
        case 'acomms'
            
            % 
            
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


