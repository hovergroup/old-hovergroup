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

Rd = [10 0;0 10*pi/180] ;  % sensor noise covar. for discrete system
Cd = [1 0 0 0;  %y 
       0 1 0 0];  %ydot
Cd1 = [1 0 0 0];
Cd2 = [0 1 0 0];

Ad =   [1.00    6.00   5.77   4.49;
  0.00    1.00   0.92   0.88;
  0.00    0.00   0.01  -0.03;
  0.00    0.00   0.03   0.04]; 

 Gd = [0.60    1.80   1.63   1.08;
  0.00    0.60   0.58   0.45;
  0.00    0.00   0.09   0.09;
  0.00    0.00  -0.10  -0.00];

Qd = 1;

iQ = inv(Gd*Qd*Gd');
   
% (x1,y1) (trackline endpoint 1, user input)
% (x2,y2) (trackline endpoint 2, user input)
% h1 = trackline heading in radians

x1 = 0;
y1 = 0;
x2 = 10;
y2 = 10;
h1 = 1;

a = (y1-y2);
b = (x2-x1);
c = (-y1*(x2-x1)+x1*(y2-y1));

z1 = heading * pi/180 - h1;
z2 = abs(a*GPSx+b*GPSy+c)/sqrt(a^2+b^2);

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