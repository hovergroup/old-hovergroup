function [dx]=kayak_continuous(t,x,A,B,Bnoise,u,wvec,twvec)
% continuous-time kayak simulation
% for use with MPC sim

% 8/20/2012 - changed to use ERROR (removed Bin,xDes)

a=find((twvec+1)>t);
w = wvec(:,a(1));
dx = A*(x)+B*u+Bnoise*w; %+Bin*xDes



%{
%system parameters
a=0.1;  %damping
b=1;  %x coefficient (k)
T=100;
%generate input u
if t<T/2
    u=1;
elseif t>=T/2 
    u=0;
end
 
A=[-a -b;1 0];
B=[u;0];  %use u0 for impulse, and u for 1d input
%system model to be solved
blank=A*x+B;
%}