function eDes= computeMPCInputs(n,N,T,desBearing,loopIt)
% compute MPC inputs
% computes eDes based on desBearing and loopIt
% function eDes = computeMPCInputs(N,T,desBearing,loopIt)

% BR, 8/19/2012

% changes
%{
- 8/19/2012: changed '3' to 'n-1' for heading state (work with both sys)

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
    pad = N-loopIt
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


end