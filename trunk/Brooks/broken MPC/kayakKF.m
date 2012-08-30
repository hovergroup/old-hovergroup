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
Ad=sys.Ad;
Bd=sys.Bd;
Cd=sys.Cd;
Bdnoise=sys.Bdnoise;

Rd = params.Rkf;
Qd = params.Qkfd;

% coord frame xform for change in desired heading
ehat(n) = ehat(n)*sin(deg2rad(90 - dDesHeading));
ehat(n-1) = ehat(n-1)-dDesHeading;

% IF syss='crossTrack_CLheading'
ehat(1) = ehat(1) - dDesHeading;

% wrap ehat(n-1) to +/- 180 deg
if(ehat(n-1) > 180)
    ehat(n-1) = ehat(n-1) - 360;
end
if(ehat(n-1) < (-180))
    ehat(n-1) = ehat(n-1) + 360;
end

xPredict = Ad*ehat + Bd*u;
PPredict = Ad*P*Ad' + Bdnoise*Qd*Bdnoise';

%K = PPredict*Cd'*inv(Cd*PPredict*Cd' + Rd);
K = (PPredict*Cd')/(Cd*PPredict*Cd' + Rd);

eHatNew = xPredict + K*(z - Cd*ehat); 
PNew = (eye(n,n) - K*Cd)*PPredict;

