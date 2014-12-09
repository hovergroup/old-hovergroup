%function to create control commands in binary form for robot arm
clc
clear all

s = serial('COM1')
set(s,'BaudRate',115200)
set(s,'ByteOrder','bigEndian')
get(s)

fopen(s)

% %message to do nothing
%command3 =['E7';'FF';'FF';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'49';'E5'];
%command3 =['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'34';'E5'];
command3 =['E7';'FF';'FF';'00';'00';'02';'FF';'FF';'0F';'FF';'0F';'FF';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'E7';'17';'70';'0A';'01';'13';'0E';'00';'00';'40';'E5'];

command4 = uint8(hex2dec(command3));
sum1 = sum(command4(1:49)); %check sum
command4(50) = mod(sum1,255)-floor(sum1/255);

i=0;
while i<5
fwrite(s,command4)
i=i+1;
pause(0.06)
%out = fread(s,s.BytesAvailable)
end

pause(0.1)
% readasync(s) %sensor output from robot
% s.BytesAvailable
% 
get(s)

%out2 = fread(s,s.BytesAvailable,'uint8')

fclose(s)
delete(s)
clear s










