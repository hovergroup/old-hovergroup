function [eHatNew PNew] = kayakKF(sys,params,z,ehat,P,u,dDesHeading)
% runs Kalman Filter for kayak cross-track system
% function [xHatNew PNew] = kayakKF(sys,params,z,xhat,P,u)

% BR, 8/6/2012

% changes
%{
- 8/14/2012 added Bin for setpt
- 8/16/2012: removed Bin for setpt (KF now fed error)
        - added coord frame xform at kinks (with dDesHeading input)
- 8/19/2012: changed to n, n-1 for xtrack and heading states (support syss)
-

%}

% grab parameters
n=sys.n;
%m=sys.m;
Ad=sys.Ad;
Bd=sys.Bd;
%Bin=sys.Bdin;
Cd=sys.Cd;
Bdnoise=sys.Bdnoise;

%speed=sys.speed;
%dt=sys.dt;
%angle2speed=sys.angle2speed;
%slewRate=sys.slewRate;

Rd = params.Rkf;
Qd = params.Qkfd;

% coord frame xform for change in desired heading
ehat(n) = ehat(n)*sin(deg2rad(90 - dDesHeading));
ehat(n-1) = ehat(n-1)-dDesHeading;

% wrap ehat(3) to +/- 180 deg
if(ehat(n-1) > 180)
    ehat(n-1) = ehat(n-1) - 360;
end
if(ehat(n-1) < (-180))
    ehat(n-1) = ehat(n-1) + 360;
end

%xPredict = Ad*xhat + Bin*xDes + Bd*u;
xPredict = Ad*ehat + Bd*u;
PPredict = Ad*P*Ad' + Bdnoise*Qd*Bdnoise';
K = PPredict*Cd'*inv(Cd*PPredict*Cd' + Rd);
eHatNew = xPredict + K*(z - Cd*ehat); 
PNew = (eye(n,n) - K*Cd)*PPredict;

