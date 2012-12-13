% design quantization encoder and decoder for TULiP

% BR, 11/9/2012

% note - this designs a symmetric quantizer, with bins on EITHER SIDE OF
% ZERO (origin is offset by smallest bin)

% b ^ (nBins/2) = max;

clear all;close all;clc

nBins = 8;
min = 0;
max = 100;
center = 50;
domain = max - min;

% generate bin sizes

% smallest bin set heuristically
bp(1) = 3;

%quantization density (see fu)
%this set via trial/error 
%(although probably a closed-form solution based on bp(1)and max)
rho = 0.4775
delta = (1-rho)/(1+rho)

bp(2) = ((1+delta)/(1-delta))*bp(1);
bp(3) = (1+delta)/(1-delta)*bp(2);
bp(4) = (1+delta)/(1-delta)*bp(3);

b = cumsum(bp);

% (zero-centered bin:)
% rho = 0.87
% delta = (1-rho)/(1+rho)
% 
% b(2) = (1+delta)/(1-delta)*b(1)+b(1);
% b(3) = (1+delta)/(1-delta)*b(2)+b(2);
% b(4) = (1+delta)/(1-delta)*b(3)+b(3);

% construct bin edges
br = [ (center - [fliplr(b)]) (center + [0 b])]

%% plot
% sector bounds
lowfac = 1/(1+delta);
highfac = 1/(1-delta);

v = linspace(-2*b(1),50,500);
figure
plot(v,(1+delta)*v)
hold on
plot(v+b(1),(1-delta)*v)

plot([0 b(1)],[0 0],'k')
plot([b(1) b(1)],[0 (1+delta)*b(1)],'k')
for i = 1:(nBins/2-1)
    plot([b(i) b(i+1)],[(1+delta)*b(i) (1+delta)*b(i)],'k')
    plot([b(i+1) b(i+1)],[(1+delta)*b(i) (1+delta)*b(i+1)],'k')
end

% draw mid slope through centroids... 
% a bit funky because smallest bin has edge at zero
midSlope = -(0.5*(0 + (1+delta)*b(1)) - 0.5*((1+delta)*b(3)+(1+delta)*b(4)))/(b(4)-b(1));
yInt = -(1-delta)*b(1)/2;
midLine = yInt + midSlope.*v;
plot(v,midLine,'k--')

centers = ((1+delta)*[0 b(1:3)] - yInt)/midSlope;
plot(centers,(1+delta)*[0 b(1:3)],'b*','MarkerSize',10)

axis equal

centers = [center - [fliplr(centers)] (center + centers)]



    


