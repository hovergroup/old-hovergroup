%------INIT ONCE------%
clear all;close all;clc
wp = importdata('relay_waypoints.txt');

cycle_counter = 0;
paused_now = 0;
waiting = 0;
paused = 0;
obs_per_location = 10;
max_cycles = 1;

rr_current_means = ones(9,1);
rr_reward_by_point = cell(9,1);
rr_c_reward_by_point = cell(9,1);
rr_c_reward = 0;
rr_c_reward_times = 0;
rr_r_decisions = [];
rr_means = cell(9,1);
r = 1;
alphas = zeros(9,1);
betas = zeros(9,1);
rr_obs = 0;

%-----IMATLAB-----%
moosDB = 'targ_shoreside.moos';
pathName = '/home/josh/hovergroup/ivp-extend/missions/josh/MABSC';

varList = {'RELAY_RESULT_MATLAB','MATLAB_PAUSE'};

old = cd(pathName);
iMatlab('init','CONFIG_FILE',moosDB);
garbageMail = iMatlab('MOOS_MAIL_RX');
cd(old);
readTimeout = 15;

%-----POST WAYPOINTS-----%
for k=1:9
    wp_msg = ['type=gateway,x=' num2str(wp(k,1)) ',y=' num2str(wp(k,2)) ',scale=4.3,label=' num2str(k)...
        ',color=green,width=4.5'];
    iMatlab('MOOS_MAIL_TX','VIEW_MARKER',wp_msg);
end

iMatlab('MOOS_MAIL_TX','MATLAB_PAUSE',0);
iMatlab('MOOS_MAIL_TX','RELAY_WAYPOINT',[num2str(wp(r,1)) ',' num2str(wp(r,2))]);
disp('Starting Cycle')

%-----ITERATE------%
while cycle_counter < max_cycles
    
    if waiting
        tstart = tic;
        waiting = 0;
        disp('Starting Data Wait')
    end
    
    data = parseObservations(varList,readTimeout);
    
    paused = data.MATLAB_PAUSE==1;
    if paused && ~paused_now
        pstart = tic;
        paused_now = 1;
        disp('Starting Pause')
    end
    
    if strcmp(data.status,'timeout')
        disp('Data Listen Timeout')
    elseif data.status(1)==1 && ~pause
        if paused_now
            paused_time = toc(pstart);
            paused_now = 0;
            disp('Ending Pause')
            disp(['Paused for: ' num2str(paused_time)]);
        else
            paused_time = 0;
            disp('No Pause Detected')
        end
        
        %Get Experimental Data
        if data.RELAY_RESULT_MATLAB == 1
            alphas(r) = alphas(r)+1 ;
            disp('Relay Success!')
        else
            betas(r) = betas(r)+1;
            disp('Relay Failure :<')
        end
    end
    
    rr_reward_by_point{r} = vertcat(rr_reward_by_point{r},data.RELAY_RESULT_MATLAB);
    rr_c_reward_by_point{r} = vertcat(rr_c_reward_by_point{r},sum(rr_reward_by_point{r}));
    rr_current_means(r) = sum(rr_reward_by_point{r})/length(rr_reward_by_point{r});
    rr_means{r} = vertcat(rr_means{r},rr_current_means(r));
    
    rr_c_reward = vertcat(rr_c_reward,rr_c_reward(end)+data.RELAY_RESULT_MATLAB);
    rr_c_reward_times = vertcat(rr_c_reward_times,rr_c_reward_times(end)+toc(tstart)-paused_time);
    
    rr_obs = rr_obs+1;
    
    if mod(rr_obs,obs_per_location)==0
        if r<9
            r = r+1;
        else
            r=1;
            cycle_counter = cycle_counter +1;
        end
        rr_obs = 0;
    end
    
    iMatlab('MOOS_MAIL_TX','RELAY_WAYPOINT',[num2str(wp(r,1)) ',' num2str(wp(r,2))]);
    disp(['Sent Waypoint ' num2str(r)])
    
    save SCRoundRobin.mat
    disp('Saved Data')
    
end