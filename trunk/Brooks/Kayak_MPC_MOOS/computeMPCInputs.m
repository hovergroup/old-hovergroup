function eDes = computeMPCInputs(n,N,T,desBearing,loopIt)
% compute MPC inputs
% computes eDes based on desBearing and loopIt
% function eDes = computeMPCInputs(N,T,desBearing,loopIt)

% BR, 8/19/2012

% changes
%{
- 8/19/2012: changed '3' to 'n-1' for heading state (work with both sys)
- 8/20/2012: change to "bump" eDes for CL heading
- 8/29/2012: changed last state desired to 0
- 8/30/2012: changed to eDesBearingLocal, construct eDes
- 9/2/2012: changed to compute dDesBearingLocal from eDesBearingLocal

%}

eDes = zeros(n,T+2);
fprintf('Step %i, current desired bearing = %f \n\n',...
    loopIt,desBearing(loopIt))

%%%%%%%%%
% itstart = loopIt-1;
% itend = loopIt+T+1;

itstart = loopIt-2;
itend = loopIt+T+1;

%%%%%%%%%
desBearingLocal = [];
if(itstart>=N)
    itstart = N;
end
if(itstart<1)
    desBearingLocal = [desBearing(1:1-itstart)];
    itstart = 1;
end
if(itend<=N)
        desBearingLocal = [desBearingLocal desBearing(itstart:itend)];
    else
        desBearingLocal = [desBearingLocal desBearing(itstart:N) desBearing(N)*ones(1,T+3-N+loopIt-2)];
end

eDesBearingLocal = desBearingLocal - ones(size(desBearingLocal))*desBearing(itstart);

% wrap eDes to +/- 180 deg
for j = 1:(T+2)
    if(eDesBearingLocal(j) > 180)
        eDesBearingLocal(j) = eDesBearingLocal(j) - 360;
    end
    if(eDesBearingLocal(j) < (-180))
        eDesBearingLocal(j) = eDesBearingLocal(j) + 360;
    end
end

%%%%%%%%%%%%%%%%%%%%%%%
dDesBearingLocal = eDesBearingLocal(2:end) - eDesBearingLocal(1:end-1);
eDes(n-1,:) = -dDesBearingLocal(2:end);
eDes(n,:) = dDesBearingLocal(2:end);

%eDes(1,:) = -[dDesBearingLocal(3:end) 0];

end