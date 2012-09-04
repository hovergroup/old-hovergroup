function [dx]=kayak_continuous(t,x,A,B,Bnoise,u,wvec,twvec)
% continuous-time kayak simulation
% for use with MPC sim

% 8/20/2012 - changed to use ERROR (removed Bin,xDes)

a=find((twvec+1)>t);
w = wvec(:,a(1));
dx = A*(x)+B*u+Bnoise*w; 