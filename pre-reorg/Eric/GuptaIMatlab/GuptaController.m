%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%Code that takes Gupta states as inputs, gives 
%thruster angle control command
%
%EWG Aug 2012
%u, YC, YsaveC, Psi need to be initialed before algorithm run
%use different variable names for Y, Ysave, Yold than GuptaEncode 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [ucontrol,packet, state] = GuptaController(Ginfo1control, Ginfo2control, p)

global u YC YsaveC Psi Yold uold xhatGupta
global Ad Gd Bd Klqr

Rd = [10 0;0 10*pi/180] ;  % sensor noise covar. for discrete system
Cd = [1 0 0 0;  %y 
       0 1 0 0];  %ydot

Qd = 1;

iQ = inv(Gd*Qd*Gd');
YC
YoldC = YC;
YC = YC+Cd'*inv(Rd)*Cd;
gamma = YoldC*Ad*inv(YsaveC);
Psi = Yold*Bd*uold + gamma*Psi;

if rand<p %packet gets through
    xhatGupta = inv(YC)*(Ginfo1control+Ginfo2control+Psi);
    packet=1;
else %(packet didn't get through)
    xhatGupta = Ad*xhatGupta + Bd*u;
    packet=0;
end
state = xhatGupta;

u = -Klqr*xhatGupta; 

YsaveC = YC;
YC = iQ - iQ*Ad*inv(Ad'*iQ*Ad+YC)*Ad'*iQ;

u1 = u*180/pi %thruster angle in degrees
        if u1 >45
            u1 = 45;
        end
        if u1 < -45
            u1 = -45;
        end
ucontrol = u1; 

