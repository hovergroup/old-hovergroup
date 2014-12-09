function [uPlanBuffered kPlan] = simPacketLossMPC(uPlanNow,uPlanBuffered,kPlan,ifPLoss)
% simulates packet loss and buffers MPC control plans
% eventually a version of this will be in MOOS on the kayak 
%   (listening to acomms)

% BR, 8/19/2012

% changelog
%{
- changed to have packet success as input (instead of prob pLoss)

%}

T = length(uPlanNow);

%if(rand>probPLoss)
if(~ifPLoss)
    
    % no packet loss
    uPlanBuffered = uPlanNow;
    kPlan = 1;
    
else
    
    % packet is lost
    fprintf('\n\nPACKET LOST\n\n')
    kPlan = kPlan + 1;
    if(kPlan>T)
        disp('WARNING: PACKET NOT RECEIVED FOR T STEPS')
        disp('RESETTING uPLAN TO ZEROS')
        uPlanBuffered = zeros(size(uPlanNow));
        kPlan = 1;
    else
        %uPlanBuffered = uPlanBuffered;
    end
        
end
