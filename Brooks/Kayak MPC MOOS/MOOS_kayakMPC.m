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

%}
clear all
close all
clc
format compact

% configure MPC parameters
configureKayakMPC;

% init
xDes = zeros(n,T+2);eDes = zeros(n,T+2);uPlan = zeros(m,T);
lenMPC_STR=100;MPC_STR = char(97*ones(1,100));
initializeMOOS_MPC;

mpcStart = tic;
loopIt=1;
% start loop  (breaks when MPC_STOP==1)
while(~mpc_stop)
    
    loopStart = tic;
    step = floor(toc(mpcStart)/dt)+1;
    fprintf('Step: %i, loopIt: %i\n\n',step,loopIt)
    
    % grab new estimate (after first loop)
    if(loopIt>1)
        % cross-track and heading ERROR relative to desBearing(step)
        [eEst mpc_stop] = parseMPC_XEST;
    end
    xHat = eEst + [0 0 desBearing(loopIt-1) 0]';
    fprintf('actual est heading: %f [deg]\n',xHat(3))
    fprintf('cross-track error: %f [m]\n\n',Cd(2,4)*eEst(4))
    
    % create inputs to MPC: eDes, uPrev:
    eDes = computeMPCInputs(N,T,desBearing,loopIt);
    disp(eDes(3,:))
    
    % solve MPC - xEst and previous control are inputs
    [uPlan tMPC] = solveKayakMPC(sys,eEst,MPCparams,uDelay,uPlan(:,1),eDes);
    fprintf('Computed plan: \n')
    disp(uPlan)

    % (encode, quantize plan)
    % send plan (just control to start)
    send = uPlan(:,1)+rOff;
    
    % DELAY (simulate acomms...)
    while(toc(loopStart)<(dt-.005))
        pause(0.005)
    end

    iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',send);
    fprintf('\n\nSending rudder = %f \n\n\n',send);
    
    % string to log
    MPC_STR(1:5) = 'uPlan';
    lenU = 8;
    for k = 1:T
        MPC_STR((6+((k-1)*lenU)):(5+(k*lenU))) = sprintf(',%+07.3f',uPlan(k));  
        %[MPC_STR ',' num2str(uPlan(k))];
    end
    MPC_STR(6+(k*lenU):lenMPC_STR) = sprintf(',tMPC,%09.3f',tMPC);
    
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

cd(old)


