% design quantization encoder and decoder for TULiP

% BR, 11/9/2012



% b ^ (nBins/2) = max;

nBins = 10;
min = 0;
max = sqrt(310^2 + 310^2);
center = 50;
domain = max - min;

rho = 0.3
delta = (1-rho)/(1+rho)
lowfac = 1/(1+delta);
highfac = 1/(1-delta);

v = linspace(0,3001,500);
figure
plot(v,(1+delta)*v)
hold on
plot(v,(1-delta)*v)

axis square

b(1) = 5;
b(2) = (1+delta)/(1-delta)*b(1);
b(3) = (1+delta)/(1-delta)*b(2);
b(4) = (1+delta)/(1-delta)*b(3);

br = center + b


%u0 = 5;
%for i = 1:nBins/2
%    u(i) = rho^i*u0;
%end


%b = fminunc(@(b) b^(nBins/2)-1,2)
%plot(0:nBins/2,b.^([0:nBins/2]));

% levels = 4;
% eps = 
% interval = [


% each bin:  "center" (in logspace), and edges
%for i = 1:nBins
    


