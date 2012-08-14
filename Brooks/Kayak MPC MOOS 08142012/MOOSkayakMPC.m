% Model Predictive Control for cross-track error
% Hovergroup kayaks, using MOOS, iMatlab
% Solved with CVX

% BR, 8/13/2012

% changes
%{
- 8/14/2012: added set point stuff (cut out heading error wrap for now)
%}

% configure MPC parameters
configureKayakMPC;

% init
xDes = zeros(n,T+2);xDes(3,:)=desBearing(1:T+2);


% configure MOOS/iMatlab parameters

moosDB = 'terra.moos';
pathName = '/home/josh/hovergroup/ivp-extend/brooks/missions/';
cd(pathName)
iMatlab('init','CONFIG_FILE',moosDB);
% note -
% subscribe to
% 'MPC_XEST'
% 'MPC_STOP'
% 'GPS_X'
% 'GPS_Y'
% 'COMPASS_HEADING_FILTERED'
%
% (others?)


stateReadTimeout = 2;   % time to wait for reading state...

% wait for MOOS side to start, and get initial X, Y, H
MPC_STOP=1;
gotStartPos=0;
while((MPC_STOP || ~gotStartPos))
    mail = iMatlab('MOOS_MAIL_RX');
    messages = length(mail);
    for i=1:messages
        key = mail(m).KEY;
        if(strcmp(key,'MPC_STOP'))
            if(strcmp(mail(m).STR,'GO'))
                MPC_STOP=0;
            end
        elseif(strcmp(key,'GPS_X'))
            x0 = mail(m).DBL;
            gotX0 = 1;
        elseif(strcmp(key,'GPS_Y'))
            y0 = mail(m).DBL;
            gotY0 = 1;
        elseif(strcmp(key,'COMPASS_HEADING_FILTERED'))
            h0 = mail(m).DBL;
            gotH0 = 1;
        end
        gotStartPos=min([gotX0,gotY0,gotH0]);
        
    end
    disp('Waiting for MOOS')
    pause(1)
end
mpcStart = tic;

% initial start position
hDes = atan2((y0-y(1)),(x0-x(1)));
hDes = rad2deg(hDes);
bearing = 90 - hDes;
if(bearing<0)
    bearing = bearing+360;
end
if(bearing>360)
    bearing = bearing - 360;
end
%eh0 = bearing-h0;
xEst = [0;0;bearing;0];


loopIt=1;
xEst=zeros(4,1);
% start loop  (breaks when MPC_STOP==1)
while(~MPC_STOP)
    
    if(loopIt>1)
        % read in KF estimate
        gotState=0;
        stateReadStart = tic;
        while(~gotState)
            mail=iMatlab('MOOS_MAIL_RX');
            messages=length(mail);
            
            if messages==0
                continue;
            end
            
            % Process messages
            
            % string of states:
            % 'MPC_XEST' = 'x1<|>x2<|>x3<|>x4<|>'
            
            key = cell(1,messages);
            val = cell(1,messages);
            str = cell(1,messages);
            for m=1:messages
                
                %msg_tic = msg_tic+1
                
                key{m}=mail(m).KEY;
                val{m}=mail(m).DBL;
                str{m}=mail(m).STR;
                
                switch key{m}
                    
                    case 'MPC_XEST'
                        
                        % parse state string
                        stateString = str{m};
                        try
                            xEst = textscan(stateString,...
                                '%f %*s %*s %f %*s %*s %f %*s %*s %f' ,...
                                'Delimiter','<|>');
                            fprintf('hddot: %f hdot: %f h: %f ex: %f \n',...
                                xEst(1),xEst(2),xEst(3),xEst(4)/C(2,4))  
                            gotState = 1;
                        catch
                            disp('error parsing state string')
                        end
                        
                    case 'MPC_STOP'
                        
                        MPC_STOP=val{m};
                        
                end
                
            end
            
            % check timeout on reading state
            if(toc(stateReadStart)>stateReadTimeout)
                disp('TIMEOUT READING IN STATES - USING OLD STATE')
                continue
            end
            
        end
    end
    
    % (desired trackline for this time)
    step = floor(toc(mpcStart)/dt)+1;
    desiredHeading = desBearing(step);
    
    
    xDes = zeros(n,T+2);
    if((i+T+1)<N)
        xDes(3,:) = desBearing(i:(i+T+1));
    else
        pad = N-i;
        xDes(3,:) = [desBearing(i:i+pad-1) desBearing(N-1)*ones(1,T+2-pad)];
    end
    xDes
    setPt=1;
    
%     % map heading from [0,360] to [-180,+180]
%     errorHeading = desiredHeading - xEst(3);
%     if(errorHeading>180)
%         errorHeading = errorHeading - 360;
%     end
%     if(errorHeading<-180)
%         errorHeading = errorHeading + 360;
%     end
    
    % replace state 3 with heading error
%    xEst(3) = errorHeading;
    
    % solve MPC - xEst and previous control are inputs
    [uPlan tMPC] = solveKayakMPC(sys,xEst,MPCparams,uDelay,uPlan(:,1),setPt,xDes);
    
    % outputs plan of desired rudder
    
    % (encode, quantize plan)
    
    % send plan (just control to start)
    send = uPlan(:,1);
    iMatlab('MOOS_MAIL_TX','DESIRED_RUDDER',send);
    fprintf('Sending rudder = %f \n',send);
    
    % send a big string of a bunch of other stuff:
    
    
    % check if at end
    if(step>=(N-1))
        MPC_STOP=1;
        disp('FINISHED TRACKLINE LIST')
    end
    
end