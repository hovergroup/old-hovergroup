function [uPlan timeMPC] = solveKayakMPC(sys,x,params,uDelay,uPrev,setPt,xDes)
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
8/14/2012: added Bin, setpoints...

%}


%setPt=0;

%uDelay=1;

% grab parameters
n=sys.n;
m=sys.m;
A=sys.Ad;
B=sys.Bd;
Bin=sys.Bdin;
%C=sys.Cd; % use CdAll for MPC... (all states in phys units)
dt=sys.dt;


ifQuiet=params.ifQuiet;
termPenalty=params.termPenalty;
T=params.T;
mu=params.mu;
Qhalf=params.Qhalf;
Rhalf=params.Rhalf;
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
cvx_precision(1e-4)

mpcstart=tic;
if(ifQuiet)
    cvx_begin quiet
else
    cvx_begin
end

if(uDelay)
    % delay on U by 1 step...
    
    if(setPt)
        
        %variables X(n,T+1) U(m,Tu)
        variables X(n,T+2) U(m,T)
        
        % max is defined in terms of ERROR????
        max((X-xDes)') <= xmax'; max(U') <= umax';
        min((X-xDes)') >= xmin'; min(U') >= umin';
        
        % NEED TO TELL MPC ABOUT DESIRED HEADING...
        
        % first state is the actual state
        X(:,1) == x;
        % cross-track is defined wrt des bearing
        X(:,2) == A*(X(:,1)) + Bin*xDes(:,1) + B*uPrev;
        X(:,3:T+2) == A*(X(:,2:T+1)) + Bin*xDes(:,2:T+1) + B*U(:,1:T);
        
    else
        %variables X(n,T+1) U(m,Tu)
        variables X(n,T+2) U(m,T)
        
        % max is defined in terms of ERROR????
        max((X)') <= xmax'; max(U') <= umax';
        min((X)') >= xmin'; min(U') >= umin';
        
        X(:,1) == x;
        X(:,2) == A*X(:,1) + B*uPrev;
        X(:,3:T+2) == A*X(:,2:T+1) + B*U(:,1:T);
    end
    
else
    variables X(n,T+1) U(m,T)
    max(X') <= xmax'; max(U') <= umax';
    min(X') >= xmin'; min(U') >= umin';
    X(:,2:T+1) == A*X(:,1:T)+B*U;
    X(:,1) == x;
    
end



if(~termPenalty)
    (X(:,T+1)-xDes(:,T+1)) == 0;
end

% could add -xDes, but equal to zero
% constrain perpendicular speed to be fraction of speed*dt
(X(4,2:T+1) - X(4,1:T))*C(4,4) <= speed*dt*angle2speed;
(X(4,2:T+1) - X(4,1:T))*C(4,4) >= -speed*dt*angle2speed;

% slew rate constraints
if(T~=1)
    (U(1,2:T) - U(1,1:T-1)) <= slewRate;
    (U(1,2:T) - U(1,1:T-1)) >= -slewRate;
end

if(termPenalty)
    % cost fcn defined wrt to desired bearing
    if(uDelay)
        
        if(setPt)
            
            minimize (norm([Qhalf*(X(:,1:T+1)-xDes(:,1:T+1))],'fro')...
                + (X(:,T+2)-xDes(:,T+2))'*P*(X(:,T+2)-xDes(:,T+2)) + mu*norm(U,1))
            
        else
            
            minimize (norm([Qhalf*(X(:,1:T+1))],'fro')...
                + (X(:,T+2))'*P*(X(:,T+2)) + mu*norm(U,1))
        end
        
        
    end
    % make more options with uDelay and not...?
elseif(~termPenalty)
    minimize (norm([Qhalf*X(:,1:T); Rhalf*U],'fro')...
        + mu*norm(U,1))
end

cvx_end

timeMPC=toc(mpcstart);
uPlan = U;
