% Hovergroup kayaks cross-track error model data
% for use with KF, MPC, etc.  

% dt = 0.5 sec
% BR, 8/13/2012


% discrete-time A matrix
Ad = [  0.4705   -0.5748         0         0; 
        0.3685    0.8390         0         0;
        0.1032    0.4717    1.0000         0;
        0.0181    0.1214    0.5000    1.0000];

% control input is rudder angle
Bud = [ 0.3685;
        0.1032;
        0.0181;
        0.0023];


% Outputs are heading, cross-track error
Cd = [  0         0    1.0000         0;
        0         0         0    0.0349];

% noise input (for use in KF)
Bdnoise = [     0.3685   -0.1610         0         0;
                0.1032    0.4717         0         0;
                0.0181    0.1214    0.5000         0;
                0.0023    0.0205    0.1250    0.5000];

% Qkfd (process noise covariance)
Qkfd =[ 0     0     0     0;
        0     0     0     0;
        0     0     6     0;
        0     0     0     2];


% Rkfd (measurement noise covariance)
Rkfd = [    3     0;
            0     5];
