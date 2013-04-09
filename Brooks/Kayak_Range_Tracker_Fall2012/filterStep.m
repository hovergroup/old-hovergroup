% Forward step for a sigma-point KF; see notes in the
% header program sigmaPointTracker.m.  This routine does
% both evolution and update steps.  The measurement vector z
% is assumed to have been taken with agent locations
% [XAgent,YAgent] based on the OLD time step's updated xhat:
% these locations are defined completely outside this routine.
%

% FSH MIT MechE October 2012

% changelog (BR)
%{
- added global targetSpeed
-
%}

function [xhat,P] = filterStep(xhat,P,z,XAgent,YAgent,...
    dim,s1,s2,s3,w,vol,Q,dt,R)

global localNoise; % used to pass process noise into ode45
%global targetSpeed; % speed of target (in filterDeriv)

options = odeset('RelTol',1e-12,'AbsTol',1e-12);
n = length(s1) ; % how many quadrature points in each direction
nAgents = length(XAgent) ; % number of agents

% Through all the abscissas ...
sP = sqrtm(P) ;
for i = 1:n,
    for j = 1:n,
        for k = 1:n,
            % get the current sigma point
            dum = sP * [s1(i,j,k) s2(i,j,k) s3(i,j,k)]' + xhat ;
            t1(i,j,k) = dum(1) ;
            t2(i,j,k) = dum(2) ;
            t3(i,j,k) = dum(3) ;
            % propogate
            localNoise = sqrt(Q)*randn;
            [~,u] = ode45('filterDeriv',[0 dt],...
                [t1(i,j,k) t2(i,j,k) t3(i,j,k)],options) ;
            % ... and record the prior
            u1(i,j,k) = u(end,1);
            u2(i,j,k) = u(end,2);
            u3(i,j,k) = u(end,3) ;
        end;
    end;
end;

% get the prior state mean via quadrature
xhat = zeros(dim,1);
for i = 1:n,
    for j = 1:n,
        for k = 1:n,
            xhat = xhat + w(i)*w(j)*w(k)*[u1(i,j,k) u2(i,j,k) u3(i,j,k)]' ;
        end;
    end;
end;
xhat = xhat/vol ;

% get the prior state covariance via quadrature
P  = zeros(dim,dim);
for i = 1:n,
    for j = 1:n,
        for k = 1:n,
            P = P + w(i)*w(j)*w(k)*([u1(i,j,k) u2(i,j,k) u3(i,j,k)]'-xhat)*...
                ([u1(i,j,k) u2(i,j,k) u3(i,j,k)]-xhat') ;
        end;
    end;
end;
P = P/vol ;

% estimate the observation from xhat
for i = 1:nAgents,
    zhat(i,1) = sqrt((xhat(2)-XAgent(i))^2 + (xhat(3)-YAgent(i))^2) ;
end;

% obs. gradient, gain, and updated xhat and covariance
for i = 1:nAgents,
    Hk(i,1:3) = [0 1/2/zhat(i)*2*(xhat(2)-XAgent(i)) ...
        1/2/zhat(i)*2*(xhat(3)-YAgent(i))] ;
end;
Kk = P*Hk'*inv(Hk*P*Hk'+R); 
xhat = xhat + Kk*(z-zhat);
P = (eye(dim,dim)-Kk*Hk)*P;

% end
    