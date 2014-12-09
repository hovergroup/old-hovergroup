function R = decodeFollower(bin,binSet)
% decodes follower range from quantizer bin to range
% currently set up for 8-bit log quantizer
% bins are 0-7
% binSet: type of decoding
% 3: log, centered @ 50, u0=3, rho = 0.4775
% 75: log, centered @ 50, u0 = 7.5, rho = 0.75
% 0: uniform, centered at 50, 0-100 range

% BR, 11/2/2012

% changelog
%{
- 11/12/2012 hardcoded new centers for u0 = 3
- 4/9/2013: changed to vector grab 
- 4/16/2013: added binSet, 75, uniform options

%}

if(nargin<2)
    binSet = 3;
end

% bin endpoints:
%b3 = [27.6 40.7 47 50 53 59.3 72.4];
%b75= [19.1667 32.5000 42.5000 50.0000 57.5000 67.5000 80.8333];
%b0 = [12.5 25 37.5 50 62.5 75 87.5];

R75 = [11.5476 26.7857 38.2143 46.7857 53.2143 61.7857 73.2143 88.4524];
% u0 = 7.5, rho = 0.75
R3 = [18.7,36.5,45,49,51,55,63.5,81.3];   
% u0 = 3, exponent = 2, rho = 0.4775

R0 = [6.25 18.75 31.25 43.75 56.25 68.75 81.25 93.75];
% UNIFORM from 0-100

switch binSet
    case 3
        R = R3(bin+1);
    case 75
        R = R75(bin+1);
    case 0
        R = R0(bin+1);
end


%{
figure
plot(R75,'bo-')

figure
plot(
plot(R0,'bo-')


figure
plot(R3,'bo-')


%}

%{
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
%}
