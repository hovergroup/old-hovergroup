function [dataOut] = parseObservations(varList,readTimeout)
% reads in observations from iMatlab/MOOS for tracking
%
% iMatlab must be subscribed to members of varList
% (usual varList) = {'FOLLOWER_X','FOLLOWER_Y',
%   'FOLLOWER_R','FOLLOWER_PACKET',...
%    'LEADER_X','LEADER_Y','LEADER_R'};

% BR, 10/11/2012

% could probably replace key{i} with key = (etc) near bottom
% put in defaults to avoid crash and warn if missing some types of packets

% changelog:
%{
- 7/3/2013: added varList as input, (for different exps)
- 7/5/2013: fixed (i think) bug with resetting gotStates

%}

dataOut = [];
gotObs = 0;
readStart = tic;
n = length(varList);
gotStates = zeros(1,n);

while(~gotObs)
    
    mail=iMatlab('MOOS_MAIL_RX');
    
    if(length(mail))==0;continue;end
    
    %disp(length(mail))
    
    [gotStates,dataOut] = parseMail(mail,varList,gotStates,dataOut);
    
    %gotObs = min(gotStates);
    %if(gotObs)
    %    dataOut.status = 'good';
    %end
    
    dataOut.status = gotStates;
    
    if max(gotStates)
        break;
    elseif(toc(readStart)>readTimeout)
        disp('TIMEOUT READING IN STATES')
        dataOut.status = 'timeout';
        break;
    end
    
    pause(0.1)
    
end


end


function [gotStates,dataOut] = parseMail(mail,varList,gotStates,dataOut)

n = length(varList);

messages = length(mail);
key = cell(1,messages);
val = cell(1,messages);
str = cell(1,messages);
i = 1;

while(~max(gotStates))
    
    key{i}=mail(i).KEY;
    val{i}=mail(i).DBL;
    str{i}=mail(i).STR;
    
    for j = 1:n
        if(strcmp(key{i},varList{j}))
            gotStates(j) = 1;
            dataOut.(varList{j}) = val{i};
        end
    end
    
    %disp(gotStates)
        
    % if more messages to look through:
    if(i<messages)
        i = i+1;
    else
        break
    end
    
end


end


