%Function to convert moos variables GPS_X, GPS_Y, COMPASS_HEADING_FILTERED
%to Matlab variables GPSx, GPSy, heading
%
%EWG Sept 2012
%

function [heading GPSx GPSy] = parseMPC_XEST
    
    mail=iMatlab('MOOS_MAIL_RX');
    messages=length(mail);
    
 while messages<4
     pause(1);
     mail=iMatlab('MOOS_MAIL_RX');
    messages=length(mail);
     
 end
 
    %if messages==0;continue;end
    
    % string of states:
    % 'MPC_XEST' = 'x1<|>x2<|>x3<|>x4<|>'
    
    key = cell(1,messages);
    val = cell(1,messages);
    str = cell(1,messages);
    
    for i = messages-3: messages
        
        %msg_tic = msg_tic+1
        
        key{i}=mail(i).KEY;
        val{i}=mail(i).DBL;
        str{i}=mail(i).STR;
        
        switch key{i}
           case 'GPS_X'
             GPSx = val{i};
           case 'GPS_Y'
             GPSy = val{i};
           case 'COMPASS_HEADING_FILTERED'
              heading = val{i};
        end

    end

end

