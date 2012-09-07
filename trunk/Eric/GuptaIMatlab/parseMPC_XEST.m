%Function to convert moos variables GPS_X, GPS_Y, COMPASS_HEADING_FILTERED
%to Matlab variables GPSx, GPSy, heading
%
%EWG Sept 2012
%

function [heading GPSx GPSy] = parseMPC_XEST

while((~gotState))
    mail=iMatlab('MOOS_MAIL_RX');
    messages=length(mail);if messages==0;continue;end
    
    % string of states:
    % 'MPC_XEST' = 'x1<|>x2<|>x3<|>x4<|>'
    
    key = cell(1,messages);
    val = cell(1,messages);
    str = cell(1,messages);
    i = 1;
    %while(~gotState)
        
        %msg_tic = msg_tic+1
        
        key{i}=mail(i).KEY;
        val{i}=mail(i).DBL;
        str{i}=mail(i).STR;
        
        switch key{i}
            
           case 'GPS_X'
                stateString = str{i};
                GPSx = textscan(stateString,'%f');
           case 'GPS_Y'
                stateString = str{i};
                GPSy = textscan(stateString,'%f');
           case 'COMPASS_HEADING_FILTERED'
                stateString = str{i};
                heading = textscan(stateString,'%f');
        end

end


end
