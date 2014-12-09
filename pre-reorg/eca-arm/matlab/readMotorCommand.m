function [ position, current ] = readMotorCommand( command )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    offset_position = 6;
    offset_current = 10;
    width = 9;

    position = zeros(5,1);
    current = zeros(5,1);
    
    for i=1:5
         position(i) = 256*command(offset_position + (i-1)*width) + command(offset_position + (i-1)*width + 1);
         current(i) = 256*command(offset_current + (i-1)*width) + command(offset_current + (i-1)*width + 1);
    end
    
% tmp = command(6);
% for exponent=7:-1:4
%    if tmp > 2^exponent
%        tmp = tmp - 2^exponent;
%    end
% end
%     disp([tmp,command(7)])
%     
end

