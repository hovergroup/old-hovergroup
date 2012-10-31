function [xEst mpc_stop] = parseMPC_XEST
% function to find and parse MPC_XEST in MOOS mail

% BR 8/15/2012

% changes
%{
- 8/16/2012: changed mpc_stop to binary
- 9/1/2012: added number of states input, changed to options for 3 or 4
    - added sys input, scale xtrack state in fprintf
- 10/9/2012: added gotState = 1 when MPC_STOP='STOP' to break out of while
    loops properly
-

%}

% if(~((n==4) || (n==5)))
%     disp('ERROR - WRONG NUMBER OF STATES')
%     mpc_stop = 1;
%     xEst = [];
%     return
% end

n=5;

xEst = zeros(n,1);
MPC_STOP='GO';
mpc_stop=0;

% read in KF estimate
gotState=0;
stateReadStart = tic;
stateReadTimeout = 3;
%
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
                
                xE = cell(1,n);
                % parse state string
                stateString = str{i};
                try
                    if(n==4)
                        xE = textscan(stateString,...
                            '%f %*s %*s %f %*s %*s %f %*s %*s %f' ,...
                            'Delimiter','<|>');
                        xEst = [xE{1};xE{2};xE{3};xE{4}];
                        
                    elseif(n==5)
                        xE = textscan(stateString,...
                            '%f %*s %*s %f %*s %*s %f %*s %*s %f %*s %*s %f' ,...
                            'Delimiter','<|>');
                        xEst = [xE{1};xE{2};xE{3};xE{4};xE{5}];
                        
                    end
                    gotState = 1;
                catch
                    disp('error parsing state string')
                end
                
            case 'MPC_STOP'
                
                MPC_STOP=str{i};
                if(strcmp(MPC_STOP,'STOP'))
                    disp('Received MPC STOP')  
                    gotState = 1;
                    break;
                end
        end
        
        % if more messages to look through:
        if(i<messages)
            i = i+1;
        else
            continue
            % break???
        end
        
        % check timeout on reading state
        if(toc(stateReadStart)>stateReadTimeout)
            disp('TIMEOUT READING IN STATES - USING OLD STATE')
            break;
        elseif(strcmp(MPC_STOP,'STOP'))
            gotState = 1;
            break;
        end
    end

end

if(strcmp(MPC_STOP,'STOP'))
    mpc_stop=1;
end

end
