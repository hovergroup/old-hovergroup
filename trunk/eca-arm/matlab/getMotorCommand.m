%{
command2 =uint8(hex2dec([
'E7';
'FF';
'FF';
'00';
'00';
'00'; motor 1 direction - shoulder
'00'; motor 1 speed ?
'00'; motor 1 speed
'0F';
'FF';
'00';
'32';
'00';
'00';
'01'; motor2_direction - slew
'FF';
'FF';
'0F';
'FF';
'0F';
'FF';
'00';
'00';
'00'; motor 3 direction  - elbow
'00';
'00';
'0F';
'FF';
'00';
'32';
'00';
'00';
'02'; % motor 4 direction - jaw rotate / wrist
'00';
'01';
'0F';
'FF';
'00';
'32';
'00';
'00';
'00'; % motor 5 direction  - jaw grip / gripper
'00';
'00';
'0F';
'FF';
'0F';
'FF';
'00';
'34';
'E5' % footer
]));
%}


function [ command ] = getMotorCommand( motor_speed, motor_direction )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

command =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'01';'7F';'FF';'0F';'FF';'0F';'FF';'00';'00';...
'02';'7F';'FF';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'34';'E5']));
offset = 6;
width = 9;
% command(6+4*width) = uint8(motor_direction(5));
% command(7+4*width) = uint8(motor_speed(5));

 for  i=1:5
    command(offset+(i-1)*width) = uint8(motor_direction(i));
    command(offset+1+(i-1)*width) = uint8(motor_speed(i)); 
 end

command(50) = mod(sum(command(1:49)),255)-floor(sum(command(1:49))/255);

end

