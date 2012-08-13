function [xHatNew PNew] = kayakKF(sys,params,z,xhat,P,u)
% runs Kalman Filter for kayak cross-track system

% grab parameters
n=sys.n;
m=sys.m;
Ad=sys.Ad;
Bd=sys.Bd;
Cd=sys.Cd;
Bdnoise=sys.Bdnoise;

%speed=sys.speed;
%dt=sys.dt;
%angle2speed=sys.angle2speed;
%slewRate=sys.slewRate;

Rd = params.Rkf;
Qd = params.Qkfd;

xPredict = Ad*xhat + Bd*u;
PPredict = Ad*P*Ad' + Bdnoise*Qd*Bdnoise';

K = PPredict*Cd'*inv(Cd*PPredict*Cd' + Rd);
xHatNew = xPredict + K*(z - Cd*xhat); 
PNew = (eye(n,n) - K*Cd)*PPredict;

