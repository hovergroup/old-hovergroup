function R = decodeFollower(bin)
% decodes follower range from quantizer bin to range

% BR, 11/2/2012

% bin endpoints:

% (base 2, smallest increment 3)
b = [29 41 47 50 53 59 71];
min = 0;
max = 300;

%%% LATER: make fcn more flexible, find mid of logspace properly...

switch bin
    case 0
        R = 10;  
    case 1
        R = mean([b(1) b(2)]);
        
    case 2
        R = mean([b(2) b(3)]);
        
    case 3
        R = mean([b(3) b(4)]);
        
    case 4
        R = mean([b(4) b(5)]);
        
    case 5
        R = mean([b(5) b(6)]);
        
    case 6
        R = mean([b(6) b(7)]);
        
    case 7
        R = 100;
        
end

