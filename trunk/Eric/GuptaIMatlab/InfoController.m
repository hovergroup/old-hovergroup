%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%Code that takes measurements heading, GPSx, and GPSy as inputs, gives 
%thruster angle control command as output
%
%EWG Aug 2012

%u, YC, YsaveC, Psi need to be initialized before algorithm run
%use different variable names for Y, Ysave, Yold than GuptaEncode 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ucontrol = InfoController(heading, GPSx, GPSy, p)

global u YC YsaveC YoldC yhat xhatInfo

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
z = [z1;z2];

Rd = [10 0;0 10*pi/180] ;  % sensor noise covar. for discrete system
Cd = [1 0 0 0;  %y 
       0 1 0 0];  %ydot

Ad =   [1.00    6.00   5.77   4.49;
  0.00    1.00   0.92   0.88;
  0.00    0.00   0.01  -0.03;
  0.00    0.00   0.03   0.04]; 

 Bd = [12.88;
  5.34;
  1.04;
 -0.04];

 Gd = [0.60    1.80   1.63   1.08;
  0.00    0.60   0.58   0.45;
  0.00    0.00   0.09   0.09;
  0.00    0.00  -0.10  -0.00];

 Klqr = [0.04    0.30   0.29   0.24];
Qd = 1;

iQ = inv(Gd*Qd*Gd');
YoldC = YC;
YC = YC+Cd'*inv(Rd)*Cd;


if rand<p %packet gets through
    yhat = yhat + Cd'*inv(Rd)*z ;
    xhatInfo = inv(YC)*yhat;
else %(packet didn't get through)
    %don't update state
end

u = -Klqr*xhatInfo; 

YsaveC = YC;
YC = iQ - iQ*Ad*inv(Ad'*iQ*Ad+YC)*Ad'*iQ;
yhat = YC*(Ad*inv(YsaveC))*yhat + Bd*u;

ucontrol = u*180/pi; %thruster angle in degrees