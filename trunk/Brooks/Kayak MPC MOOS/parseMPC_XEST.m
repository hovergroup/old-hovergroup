function [xEst mpc_stop] = parseMPC_XEST
% function to find and parse MPC_XEST in MOOS mail

% BR 8/15/2012

% changes
%{
- 8/16/2012: changed mpc_stop to binary

%}

xEst = zeros(4,1);
MPC_STOP='GO';
mpc_stop=0;

% read in KF estimate
gotState=0;
stateReadStart = tic;
stateReadTimeout = 3;
while((~gotState))
    mail=iMatlab('MOOS_MAIL_RX');
    messages=length(mail);if messages==0;continue;end
    
    % string of states:
    % 'MPC_XEST' = 'x1<|>x2<|>x3<|>x4<|>'
    
    key = cell(1,messages);
    val = cell(1,messages);
    str = cell(1,messages);
    i = 1;
    while(~gotState)
        
        %msg_tic = msg_tic+1
        
        key{i}=mail(i).KEY;
        val{i}=mail(i).DBL;
        str{i}=mail(i).STR;
        
        switch key{i}
            
            case 'MPC_XEST'
                
                xE = cell(1,4);
                % parse state string
                stateString = str{i};
                try
                    xE = textscan(stateString,...
                        '%f %*s %*s %f %*s %*s %f %*s %*s %f' ,...
                        'Delimiter','<|>');
                    xEst = [xE{1};xE{2};xE{3};xE{4}];
                    %fprintf('hddot: %f hdot: %f h: %f [deg] ex [m]: %f \n',...
                    %    xEst(1),xEst(2),xEst(3),Cd(2,4)*xEst(4))
                    gotState = 1;
                catch
                    disp('error parsing state string')
                end
                
            case 'MPC_STOP'
                
                MPC_STOP=str{i};
                if(strcmp(MPC_STOP,'STOP'))
                    disp('Received MPC STOP')
                    break;
                end
        end
        
        % if more messages to look through:
        if(i<messages)
            i = i+1;
        else
            continue
        end
        
    end
    
    % check timeout on reading state
    if(toc(stateReadStart)>stateReadTimeout)
        disp('TIMEOUT READING IN STATES - USING OLD STATE')
        break;
    elseif(strcmp(MPC_STOP,'STOP'))
        break;
    end
    
end

if(strcmp(MPC_STOP,'STOP'))
    mpc_stop=1;
end

fprintf('State estimate\n')
fprintf('%s %f\n','ehddot', eEst(1), 'ehdot ', ...
    eEst(2), 'eh    ', eEst(3), 'ex    ', eEst(4))

end
