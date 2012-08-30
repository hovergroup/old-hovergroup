function [uPlan timeMPC] = solveKayakMPC(sys,x,params,uDelay,uPrev,xDes)
% function to solve MPC each time step for kayak cross-track model
% function [uPlan timeMPC] = solveKayakMPC(sys,x,params,uDelay,uPrev)
% inputs:
%
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
- 8/16/2012: changed delta xmin/xmax to not include X(:,2)-X(:,1) with uDelay
- 8/19/2012: changed xmin/xmax to not include X(:,1:2), also objective

%}


% grab parameters
n=sys.n;
m=sys.m;
A=sys.Ad;
B=sys.Bd;
Bin=sys.Bdin;
dt=sys.dt;

ifQuiet=params.ifQuiet;
T=params.T;
mu=params.mu;
Qhalf=params.Qhalf;
P=params.Pmpc;
angle2speed=params.angle2speed;
slewRate=params.slewRate;
speed=params.speed;
umax=params.umax;
umin=params.umin;
xmax=params.xmax;
xmin=params.xmin;
C=params.CdAll;

% setup and call CVX

%cvx_precision(max(min(abs(x))/10,1e-6))
cvx_precision(1e-1)

mpcstart=tic;
if(ifQuiet)
    cvx_begin quiet
else
    cvx_begin
end

if(uDelay)
    % delay on U by 1 step...
    
    variables X(n,T+2) U(m,T)
    
    % max is defined in terms of ERROR
    max((X(:,3:T+2)-xDes(:,3:T+2))') <= xmax';
    max(U') <= umax';
    min((X(:,3:T+2)-xDes(:,3:T+2))') >= xmin';
    min(U') >= umin';
    
    % **cross-track is defined wrt des bearing - Bin
    
    % first state is the actual state
    X(:,1) == x;
    % propagate using previous control to get X(t=2)
    X(:,2) == A*(X(:,1)) + Bin*xDes(:,1) + B*uPrev;
    X(:,3:T+2) == A*(X(:,2:T+1)) + Bin*xDes(:,2:T+1) + B*U;
    
    % could add -xDes, but equal to zero
    % constrain perpendicular speed to be fraction of speed*dt
    (X(n,4:T+1) - X(n,3:T))*C(n,n) <= speed*dt*angle2speed;
    (X(n,4:T+1) - X(n,3:T))*C(n,n) >= -speed*dt*angle2speed;
    
else
    
    variables X(n,T+1) U(m,T)
    max((X(:,2:T+1)-xDes(:,2:T+1))') <= xmax'; max(U') <= umax';
    min((X(:,2:T+1)-xDes(:,2:T+1))') >= xmin'; min(U') >= umin';
    
    X(:,2:T+1) == A*X(:,1:T)+B*U;
    X(:,1) == x;
    
    % could add -xDes, but equal to zero
    % constrain perpendicular speed to be fraction of speed*dt
    (X(n,2:T+1) - X(n,1:T))*C(n,n) <= speed*dt*angle2speed;
    (X(n,2:T+1) - X(n,1:T))*C(n,n) >= -speed*dt*angle2speed;
    
end



% slew rate constraints
if(T~=1)
    (U(1,2:T) - U(1,1:T-1)) <= slewRate;
    (U(1,2:T) - U(1,1:T-1)) >= -slewRate;
end

% cost fcn defined wrt to desired bearing
if(uDelay)
    minimize (norm([Qhalf*(X(:,3:T+1)-xDes(:,3:T+1))],'fro')...
        + (X(:,T+2)-xDes(:,T+2))'*P*(X(:,T+2)-xDes(:,T+2)) + mu*norm(U,1))
else
    minimize (norm([Qhalf*(X(:,2:T)-xDes(:,2:T))],'fro')...
        + (X(:,T+1)-xDes(:,T+1))'*P*(X(:,T+1)-xDes(:,T+1)) + mu*norm(U,1))
end


cvx_end

timeMPC=toc(mpcstart);
uPlan = U;
