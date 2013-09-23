function [ position_commanded ] = normalizePosition( position_read )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    if(position_read > 2^15)    % 32k-64k
        position_commanded = position_read - 2^15;
    elseif (position_read >= 0)  % 0-32k
        position_commanded = position_read + 2^15;
    else
        disp(['Something went wrong: position_read = ', num2str(position_read)]);
    end

end

