function [uPlanBuffered kPlan ifPLoss] = simPacketLossMPC(uPlanNow,uPlanBuffered,kPlan,probPLoss)

if((probPLoss<0) || (probPLoss>1))
    disp('WARNING - INVALID PACKET LOSS PROBABILITY')
end
T = length(uPlanNow);

if(rand>probPLoss)
    
    % no packet loss
    uPlanBuffered = uPlanNow;
    kPlan = 1;
    ifPLoss = 0;
    
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
        uPlanBuffered = uPlanBuffered;
    end
    
    ifPLoss = 1;
end
