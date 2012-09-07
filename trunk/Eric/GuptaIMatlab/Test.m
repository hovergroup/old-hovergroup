%function to test Guptacontrol, Guptaencode
global u YC YsaveC Psi Yold uold xhatGupta
global Y Ysave Ginfo1 Ginfo2 yhat xhatInfo

u=0;
YC=eye(4,4);
YsaveC = YC;
Psi = zeros(4,1);
Yold = YC;
uold = u;
xhatGupta = zeros(4,1);
Y = eye(4,4);
Ysave = YsaveC;
Ginfo1 = [1;1;1;1];
Ginfo2 = [0.5;0;0;0];
yhat = zeros(4,1);
xhatInfo = zeros(4,1);

Ginfo1control = [1;1;1;1];
Ginfo2control = [0.5;0;0;0];
p = 0.5;

%GuptaController(Ginfo1control, Ginfo2control, p);

heading = 10; %degrees
GPSx = 5;
GPSy = 12;

InfoController(heading,GPSx,GPSy,p)

%[a b] = GuptaEncode(heading,GPSx,GPSy)


