function R = decodeFollower(bin)
% decodes follower range from quantizer bin to range

% BR, 11/2/2012

% changelog
%{
- hardcoded new centers for u0 = 3

%}

% bin endpoints:

b = [27.6 40.7 47 50 53 59.3 72.4];

%%% LATER: make fcn more flexible, find mid of logspace properly...
% (encode formula)

switch bin
    case 0
        R = 18.7;
    case 1
        R = 36.5;
    case 2
        R = 45;
    case 3
        R = 49;
    case 4
        R = 51;
    case 5
        R = 55;
    case 6
        R = 63.5;
    case 7
        R = 81.3;
end

