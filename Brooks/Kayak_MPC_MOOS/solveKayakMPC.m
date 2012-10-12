function [uPlan timeMPC X] = solveKayakMPC(sys,x,params,uPrev)
% function to solve MPC each time step for kayak cross-track model
% function [uPlan timeMPC] = solveKayakMPC(sys,x,params,uPrev)
% inputs:
%   x = {ePsi(k-1), evec(k) from KF}
%   evec(k) matches system description in sys
%   convention is: [derivs, heading, cross-track,(int cross track)]
% output: uPlan - a vector of size m x T with the control plan

% BR, 8/6/2012
% based on randsys_mpc.m
% EE364b, Convex Optimization II, S. Boyd, Stanford University.
% Written by Yang Wang, 04/2008

% changes:
%{
-8/14/2012: added Bin, setpoints...
-8/15 - removed setPt input (always in), got rid of hard terminal
constraint option
- 8/16/2012: changed delta xmin/xmax to not include X(:,2)-X(:,1)
- 8/19/2012: changed xmin/xmax to not include X(:,1:2), also objective
- 9/2/2012: removed slew rate constraints (with deltaPSI, just enforce umax
    and umin)
    - changed xmax to start at step 4 (takes 1 step for control to
    propagate)
    - (tried different delay eqns)
    - fixed bug with all zeros and large terminal state (using norm)
- 9/4/2012: changed to have uPrev incorporated into ePSI initial
    - added shift so that heading matches setpt.
- 10/9/2012: removed uDelay (always use), xDes (always regulate error), 
    redid propagation to handle ePsi separately (not in A)

%}


% grab parameters
n=sys.n;
m=sys.m;
A=sys.Ad;
B=sys.Bd;
dt=sys.dt;

ifQuiet=params.ifQuiet;
T=params.T;
mu=params.mu;
Qhalf=params.Qhalf;
P=params.Pmpc;
umax=params.umax;
umin=params.umin;
xmax=params.xmax;
xmin=params.xmin;


%C=params.CdAll;

% setup and call CVX

%cvx_precision(max(min(abs(x))/10,1e-6))
cvx_precision(1e-2)

mpcstart=tic;
if(ifQuiet)
    cvx_begin quiet
else
    cvx_begin
end

% delay on U by 1 step...

variables X(n,T+2) U(m,T)

% ePsi:
X(1,1) = x(1) + uPrev;    % ePsi(k)
X(1,2:T+1) = X(1,1:T) + U(1:T); % ePsi(k+1:k+T+1)

% state estimate:
X(2:n,1) = x(2:n);
% propagation:
X(2:n,2:T+2) = A*X(2:n,1:T+1) + B*X(1,1:T+1);

% max is defined in terms of ERROR
% start @ 3 b/c takes 1 step for control to take effect
max((X(:,3:T+2))') <= xmax';
min((X(:,3:T+2))') >= xmin';

max(U') <= umax';
min(U') >= umin';

% cost fcn defined wrt to desired bearing
minimize (norm([Qhalf*(X(:,3:T+1))],'fro')...
    + norm(sqrtm(P)*(X(:,T+2)),'fro') + mu*norm((U)))

cvx_end

timeMPC=toc(mpcstart);
uPlan = U;


%
if(0)
    xplot = X;
    xplot(2,:) = xplot(2,:)*sys.Cd(n);
    xplot(5,:) = xplot(5,:)*sys.Cd(n);
    
    figure
    stairs([1 2],[uPrev uPrev],'k*')
    hold on
    stairs(2:T+1,uPlan,'b-*')
    stairs(1:T+2,xplot(1,1:T+2),'r')
    stairs(0:T+1,xplot(n-1,:),'g--')
    legend('previous control (applies during step 1)',...
        'uPlan','local heading setpoint','local heading predicted')
    
    
    
    figure
    stateTitles = {'heading setpoint','int x','ehdot','eh','ex'};
    %stairs(1:T+3,xplot')
    for i = 1:n
        subplot(n,1,i)
        stairs(xplot(i,:))
        title(stateTitles{i})
    end
    
end
%





