% Derivative function to be called by filterStep.m (and SIM_range_tracker)
% kinematic model driven by noise in heading rate

% (Maybe add option for other model - integrators in x, y?)

% FSH MIT MechE October 2012

% changelog (BR)
%{
- 11/11/2012: made targetSpeed global
- 

%}

function [xdot] = filterDeriv(t,x)
global localNoise ;
global targetSpeed;

% state is [heading, Cartesian X, Cartesian Y] 

xdot(1,1) = localNoise ; % heading rate
xdot(2,1) = targetSpeed*sin(x(1)) ; % speed along Cartesian x 
xdot(3,1) = targetSpeed*cos(x(1)) ; % speed along Cartesian y

