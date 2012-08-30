function eDes= computeMPCInputs(n,N,T,syss,desBearing,loopIt)
% compute MPC inputs
% computes eDes based on desBearing and loopIt
% function eDes = computeMPCInputs(N,T,desBearing,loopIt)

% BR, 8/19/2012

% changes
%{
- 8/19/2012: changed '3' to 'n-1' for heading state (work with both sys)
- 8/20/2012: added syss input, and change to "bump" eDes for CL heading
- 8/29/2012: made last eDes all zeros (to avoid edge case with terminal
        state weighting)

%}

% compute xDes in MPC coord frame

% first construct xDes vector in bearing coord frame
xDes = zeros(n,T+2);
eDes = zeros(n,T+2);

% Note - MPC uses xDes(1) in propagation of state estimate to X(:,2),
% along with uPrev.  So xDes(1) matches timestep of uPrev

if(loopIt==1)
    % desBearing(planning step 0) = desBearing(1)
    xDes(n-1,:) = [desBearing(1) desBearing(1:T+1)];
elseif((loopIt+T)<N)
    xDes(n-1,:) = desBearing((loopIt-1):(loopIt+T));
else
    pad = N-loopIt;
    xDes(n-1,:) = [desBearing((loopIt-1):N) desBearing(N)*ones(1,T-pad)];
end
fprintf('Step %i, new desired bearing = %f \n\n',...
    loopIt,desBearing(loopIt))

% MPC coord frame is defined relative to desBearing(i-1)
% (since MPC starts planning for next step)
if(loopIt==1)
    eDes(n-1,:) = xDes(n-1,:) - ones(size(xDes(3,:)))*desBearing(1);
else
    eDes(n-1,:) = xDes(n-1,:) - ones(size(xDes(3,:)))*desBearing(loopIt-1);
end

% wrap eDes to +/- 180 deg
for j = 1:(T+2)
    if(eDes(n-1,j) > 180)
        eDes(n-1,j) = eDes(n-1,j) - 360;
    end
    if(eDes(n-1,j) < (-180))
        eDes(n-1,j) = eDes(n-1,j) + 360;
    end
end

    % quick hack to test 
    if(strcmp(syss,'crossTrack_CLheading'))  
        inds = find(eDes(n-1,:));
        
        % with Bin eye(n)
        eDes(n,:) = eDes(n-1,:);
        
        
        
        if(inds)
            eDes(n-1,(inds(1)+1):end)=zeros(1,1:(T+2-inds(1)));
        end
    end
    
    %%%% QUICK HACK FOR TERMINAL STATE WEIGHTING...
    eDes(:,end) = zeros(n,1);

    
end