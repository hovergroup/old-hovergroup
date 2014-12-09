%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%Code that takes heading and gps measurements as input, gives 
%Gupta states as outputs
%
%EWG Aug 2012
%Y, Ysave need to be initialized before algorithm starts
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [Ginfo1out Ginfo2out ] = GuptaEncode(heading, GPSx, GPSy)

global Y Ysave  Yold 
global Ginfo1 Ginfo2
global Ad Gd

Rd = [10 0;0 10*pi/180] ;  % sensor noise covar. for discrete system
Cd = [1 0 0 0;  %y 
       0 1 0 0];  %ydot
Cd1 = [1 0 0 0];
Cd2 = [0 1 0 0];


Qd = 1;

iQ = inv(Gd*Qd*Gd');
   
% (x1,y1) (trackline endpoint 1, user input)
% (x2,y2) (trackline endpoint 2, user input)
% h1 = trackline heading in radians

x1 = 20;
y1 = 0;
x2 = 20;
y2 = -800;
h1 = 180;

a = (y1-y2);
b = (x2-x1);
c = (-y1*(x2-x1)+x1*(y2-y1));

z1 = (heading - h1)*pi/180;
z2 = -(a*GPSx+b*GPSy+c)/sqrt(a^2+b^2)


Yold = Y;
Y = Y+Cd'*inv(Rd)*Cd;
gamma = Yold*Ad*inv(Ysave);

lambda1 = Cd1'*inv(Rd(1,1))*z1;
lambda2 = Cd2'*inv(Rd(2,2))*z2;
Ginfo1 = lambda1 + gamma*Ginfo1;
Ginfo2 = lambda2 + gamma*Ginfo2;

Ysave = Y;
Y = iQ - iQ*Ad*inv(Ad'*iQ*Ad+Y)*Ad'*iQ;

Ginfo1out = Ginfo1;
Ginfo2out = Ginfo2;