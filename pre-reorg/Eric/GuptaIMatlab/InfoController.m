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
global Ad Gd Bd Klqr

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

z1 = heading * pi/180 - h1;
z2 = -(a*GPSx+b*GPSy+c)/sqrt(a^2+b^2);
z = [z1;z2];

Rd = [10 0;0 10*pi/180] ;  % sensor noise covar. for discrete system
Cd = [1 0 0 0;  %y 
       0 1 0 0];  %ydot

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


u1 = u*180/pi %thruster angle in degrees
        if u1 >45
            u1 = 45;
        end
        if u1 < -45
            u1 = -45;
        end
 ucontrol = u1