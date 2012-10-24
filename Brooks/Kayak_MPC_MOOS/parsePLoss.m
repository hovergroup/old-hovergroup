function [ifPLoss] = parsePLoss(timeout,data,neededFrames,numFrames)
% function to determine if packet was successfully received
% checks for successful reception until timeout expires
% data is a STRING: if this pattern found, then OK
% neededFrames: number of times data string found in RX string (default 1)
% (now uses data - compares RX data to expected data)
% note: will return number of correct frames if data is repeated frame
% (future - use bad_frames and timing for expected posting?)


% BR 10/24

% changes
%{
-
-

%}

if(nargin==2)
    numFrames = 3;
    neededFrames = 1;
end
if(nargin==3)
    numFrames = 3;
end

checkPacket=0;
checkStart = tic;
checkTimeout = timeout;
%
while((~checkPacket))
    mail=iMatlab('MOOS_MAIL_RX');
    messages=length(mail);if messages==0;continue;end
    
    key = cell(1,messages);
    val = cell(1,messages);
    str = cell(1,messages);
    time = cell(1,messages);
    
    i = 1;
    while(~checkPacket)
                
        key{i}=mail(i).KEY;  
        val{i}=mail(i).DBL;
        str{i}=mail(i).STR;
        time{i}=mail(i).TIME;
        
        switch key{i}
            
            case 'ACOMMS_RECEIVED_DATA'
                rxdata = str{i};
                test = strfind(rxdata,data);
                % if pattern matches at all
                if(~isempty(test))
                    checkPacket = 1;
                end
                numGoodFrames = length(test);
                
            case 'ACOMMS_BAD_FRAMES'
                % currently just prints out, doesn't affect while loop
                badFrames = str{i};
                
                fprintf('BAD FRAMES: %s \n\n',badFrames)
                % need to check time...
                
%                 if(isempty(badFrames))
%                     % depends on packet sent...
%                     numGoodFrames = numFrames;
%                 else
%                     bFrames = find(badFrames~=(' '));
%                     numGoodFrames = numFrames - bFrames;
%                 end
                
                
        end
        
        % if more messages to look through:
        if(i<messages)
            i = i+1;
        else
            continue
        end
        
        % check timeout on reading state
        if(toc(checkStart)>checkTimeout)
            disp('TIMEOUT - LOST PACKET')
            checkPacket = 1;
            numGoodFrames = 0;
            break;
        end
    end

end

if(numGoodFrames>=neededFrames)
    ifPLoss = 0;
else
    ifPLoss = 1;
end

end
