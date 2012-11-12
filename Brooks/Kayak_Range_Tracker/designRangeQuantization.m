% design quantization encoder and decoder for TULiP

% BR, 11/9/2012



% b ^ (nBins/2) = max;

nBins = 8;
min = 0;
max = sqrt(310^2 + 310^2);
center = 50;

binsP = 4;
binsM = 4;



domain = max - min;

b = fminunc(@(b) b^(nBins/2)-domain/2,2)

plot(0:nBins/2,b.^([0:nBins/2]));

% each bin:  "center" (in logspace), and edges
%for i = 1:nBins
    


