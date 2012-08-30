function str = printNumFile(val,prec)
% turns fractional values into a sci. notation string with no decimals
% used for putting parameter values into filenames
% function str = printNumFile(val,prec)
% val: value to be printed
% prec: (OPTIONAL) number of decimal places to keep (default 5)
% str: string output

% BR, 3/3/2012

%{
-
-
%}

if(nargin<2)
    prec=5;
end

valAdj=val;
modVal=1;
adj=-1;

val=round((val*10^prec))/(10^prec);

while(modVal>0)
    adj=adj+1;
    valAdj=val*10^(adj-1);
    modVal=mod(valAdj,1);
end

valInt=valAdj;
pow=-(adj-1);
str=sprintf('%ie%i',valInt,pow);