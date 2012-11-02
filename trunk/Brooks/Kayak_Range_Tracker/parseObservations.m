function [dataOut] = parseObservations(readTimeout)
% reads in observations from iMatlab/MOOS for tracking
% must be subscribed to: varList = {'FOLLOWER_X','FOLLOWER_Y',
%   'FOLLOWER_R','FOLLOWER_PACKET',...
%    'LEADER_X','LEADER_Y','LEADER_R'};%%%,'LEADER_PACKET'};
    
% BR, 10/11/2012

% changelog:
%{
-
-

%}


gotObs = 0;
readStart = tic;
%readTimeout = 1;

while(~gotObs)
    
    mail=iMatlab('MOOS_MAIL_RX');
    
    if(length(mail))==0;continue;end
    
    [gotObs,dataOut] = parseMail(mail);
    
    if(toc(readStart)>readTimeout)
        disp('TIMEOUT READING IN STATES')
        dataOut.status = 'timeout';
        break;
    end
    
end

end


function [gotAllObs,dataOut] = parseMail(mail)

% [xL,yL,rL,piL,xF,yF,rF,piF]
varList = {'FOLLOWER_X','FOLLOWER_Y','FOLLOWER_R','FOLLOWER_PACKET',...
    'LEADER_X','LEADER_Y','LEADER_R'};%,'LEADER_PACKET'};
n = length(varList);

gotStates = zeros(1,n);
gotAllObs = 0;

key = cell(1,messages);
val = cell(1,messages);
str = cell(1,messages);
i = 1;

while(~gotAllObs)
    
    key{i}=mail(i).KEY;
    val{i}=mail(i).DBL;
    str{i}=mail(i).STR;
    
    for j = 1:n
        if(strcmp(key{i},varList{j}))
            gotStates(j) = 1;
            dataOut.(varList{j}) = val{i};
        end
    end
    
    % if more messages to look through:
    if(i<messages)
        i = i+1;
    else
        continue
    end
    
    gotAllObs = min(gotStates);
    
end


end


