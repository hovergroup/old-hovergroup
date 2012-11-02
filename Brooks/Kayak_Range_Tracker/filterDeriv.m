% Derivative function to be called by sigmaPointTracker.m
% FSH MIT MechE October 2012

function [xdot] = filterDeriv(t,x);
global localNoise ;

U = 1;

% state is [heading, Cartesian X, Cartesian Y] 

xdot(1,1) = localNoise ; % heading rate
xdot(2,1) = U*sin(x(1)) ; % speed along Cartesian x 
xdot(3,1) = U*cos(x(1)) ; % speed along Cartesian y