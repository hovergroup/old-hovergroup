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
- 10/9/2012: updated for next round of exps
    REMOVED TRAJECTORY-FOLLOWING PARTS
    Added some more logging
- 10/15/2012: fixed issues with internal state vs output (phys units)
scaling

%}


clear all
close all
clc
format compact


configureKayakMPC;
initializeMOOS_MPC;


%% Loop
mpcStart = tic;
loopIt=1;

uPlanSave = cell(1,N+1);
XSave = cell(1,N+1);
eEstSave = zeros(n,N+1);


diary on
% start loop  (breaks when MPC_STOP==1)
while(~mpc_stop)
    
    loopStart = tic;
    runStep = floor(toc(mpcStart)/dt)+1;
    fprintf('Step: %i, loopIt: %i\n\n',runStep,loopIt-1)
    
    % estimate is of state right when control packet arrives
    
    if(loopIt>1)
        % cross-track and heading ERROR relative to desBearing(step)
        [eEstKF mpc_stop] = parseMPC_XEST(n,sys);
        % eEstKF: intx,ehddot,ehdot,eh,ex
        
        % UPDATE SETPOINT (takes effect immediately on reception)
        % estimator knows what setpt applied
        eEst(1) = eEst(1) + u;
        
        % update all outputs (physical units)
        switch syss
            case 'crossTrack'
                eEst(2:(n+1)) = eEstKF(2:5);
            case 'crossTrack_integrator'
                eEst(n+1) = eEstKF(1);
                eEst(2:n) = eEstKF(2:5);
        end
        
        % convert to internal state scaling
        eEst(2:n+1) = sys.CdAll\eEst(2:n+1);
        
        if(0)
            fprintf('State estimate (all outputs)\n')
            fprintf('%s %f\n','ehddot', eEst(2), 'ehdot ', ...
                eEst(3), 'eh    ', eEst(4), 'ex    ', eEst(5),...
                'intx   ',eEst(6))
        end
        
        hEst = eEst(n-1) + desBearing(loopIt);
        fprintf('actual est heading: %f [deg]\n',hEst)
        
    end
    
    % next planned control action
    if(kPlan<T)
        uPrev = uPlanBuffered(:,kPlan+1);
    else
        uPrev = 0;
    end
    
    % solve MPC - xEst and previous control are inputs
    [uPlan tMPC X] = solveKayakMPC(sys,eEst,MPCparams,uPrev);
    
    % SIM PACKET LOSS (WITH DELAY)
    % buffer works on RECEIVED PLAN AT THIS TIME STEP (delayed)
    [uPlanBuffered kPlan ifPLoss] = simPacketLossMPC...
        (uPlan,uPlanBuffered,kPlan,probPLoss);
    u = uPlanBuffered(:,kPlan);     % send appropriate control action
    
    fprintf('Computed plan: \n')
    disp(uPlan)
    %fprintf('Buffered plan: \n')
    %disp(uPlanBuffered)
    
    %%%% ACTUAL CONTROL TO SEND IS HEADING SETPOINT
    uBearing = eEst(1) + desBearing(loopIt) + u;
    if(uBearing<0);uBearing = uBearing+360;end
    if(uBearing>360);uBearing = uBearing - 360;end
    fprintf('\n Next step heading controller setpoint: %f\n\n',uBearing)
    
    fprintf('uBearing: %f   u: %f      e_phi: %f       uPrev: %f\n',...
        uBearing,u,eEst(1),uPrev)
    fprintf('MPC predicted next state (from uPrev):\n')
    X(:,2)
    
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
    
    % matlab save
    uPlanSave{loopIt} = uPlan;
    XSave{loopIt} = X;
    eEstSave(:,loopIt) = eEst;
    
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

%%

figure
stairs(eEstSave(1:3,:)')

plotInds = [10 11 12 13]
%plotInds = [20 21 22];
%plotInds = [40 41 42 43];

colors = {'r','b','g','m','c','k'}
%
ns = 4;
figure
for i = 1:length(plotInds)
    k = plotInds(i);
    subplot(ns,1,1)
    stairs(uPlanSave{k},colors{i})
    hold on
    title('control plan')
    subplot(ns,1,2)
    stairs(XSave{k}(1,:),colors{i})
    hold on
    title('predicted setpoint')
    
    subplot(ns,1,3)
    stairs(XSave{k}(3,:),colors{i})
    hold on
    title('predicted heading')
    
    subplot(ns,1,4)
    stairs(XSave{k}(4,:),colors{i})
    hold on
    title('predicted cross track')
    
end
%




