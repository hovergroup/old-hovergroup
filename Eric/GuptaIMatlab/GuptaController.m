%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%Code that takes Gupta states as inputs, gives 
%thruster angle control command
%
%EWG Aug 2012
%u, YC, YsaveC, Psi need to be initialed before algorithm run
%use different variable names for Y, Ysave, Yold than GuptaEncode 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ucontrol = GuptaController(Ginfo1control, Ginfo2control, p)

global u YC YsaveC Psi Yold uold xhatGupta

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
YC
YoldC = YC;
YC = YC+Cd'*inv(Rd)*Cd;
gamma = YoldC*Ad*inv(YsaveC);
Psi = Yold*Bd*uold + gamma*Psi;

if rand<p %packet gets through
    xhatGupta = inv(YC)*(Ginfo1control+Ginfo2control+Psi);
else %(packet didn't get through)
    xhatGupta = Ad*xhatGupta + Bd*u;
end

u = -Klqr*xhatGupta; 

YsaveC = YC;
YC = iQ - iQ*Ad*inv(Ad'*iQ*Ad+YC)*Ad'*iQ;

ucontrol = u*180/pi; %thruster angle in degrees