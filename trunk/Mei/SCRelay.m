%------INIT ONCE------%
clear all;close all;clc
global Results bernoulli_coefficients

load('bernoulli_indices.mat')
load('bernoulli_coefficients.csv')
load('cc_hybrid.mat')
wp = importdata('relay_waypoints.txt');
switching_matrix = zeros(9,9);
for i=1:9
    for j=1:9
        switching_matrix(i,j) = ((wp(i,1)-wp(j,1))^2 + (wp(i,2)-wp(j,2))^2)^0.5;
    end
end
switching_matrix = floor(switching_matrix)/1.5/15;

beta = 0.95;
horizon = 5;

%FLAGS
paused_now = 0;
waiting = 1;
showfigures = 1;

%VARS
paused_time = 0;
sw_gindex = ones(9,1);
sw_current_means = ones(9,1);
sw_time_index = zeros(9,1);
sw_gindex_history = cell(9,1);
sw_mab_reward_by_point = cell(9,1);
sw_mab_c_reward_by_point = cell(9,1);
sw_mab_c_reward = 0;
sw_mab_c_reward_times = 0;
sw_r_decisions = [];
sw_mode_decisions = [];
sw_means = cell(9,1);
r_hist = 1;
r = 1;
alphas = zeros(9,1);
betas = zeros(9,1);
enum_times = [];
sw_obs = 1;

if showfigures
    figure;
    figure_handles(1) = gca;
    figure;
    figure_handles(2) = gca;
    figure;
    figure_handles(3) = gca;
    figure;
    figure_handles(4) = gca;
    figure;
    figure_handles(5) = gca;
    set(figure_handles,'fontsize',15);
end

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

iMatlab('MOOS_MAIL_TX','MATLAB_PAUSE','false');
iMatlab('MOOS_MAIL_TX','RELAY_WAYPOINT',[num2str(wp(r,1)) ',' num2str(wp(r,2))]);
disp('Starting Cycle')

%-----ITERATE------%
while 1
    if waiting
        tstart = tic;
        waiting = 0;
        disp('Starting Data Wait')
    end
    
    data = parseObservations(varList,readTimeout);    
    pause = strcmp(data.MATLAB_PAUSE,'true');
    if pause && ~paused_now 
        pstart = tic;
        paused_now = 1;
        disp('Starting Pause')
    end
    
    if data.status==1 && ~pause
        if paused_now
            paused_time = toc(pstart);
            paused_now = 0;
            disp('Ending Pause')
        else
            paused_time = 0;
            disp('No Pause Detected')
        end
        
        disp('Calculating...')
        %Get Experimental Data
        if data.RELAY_RESULT_MATLAB == 1
            alphas(r) = alphas(r)+1 ;
            disp('Relay Success!')
        else
            betas(r) = betas(r)+1;
            disp('Relay Failure :<')
        end
        
        sw_mab_reward_by_point{r} = vertcat(sw_mab_reward_by_point{r},data_point);
        sw_mab_c_reward_by_point{r} = vertcat(sw_mab_c_reward_by_point{r},sum(sw_mab_reward_by_point{r}));
        sw_current_means(r) = sum(sw_mab_reward_by_point{r})/length(sw_mab_reward_by_point{r});
        sw_means{r} = vertcat(sw_means{r},sw_current_means(r));

        sw_gindex(r) = interpolateBI(alphas(r),betas(r));
        sw_gindex_history{r} = vertcat(sw_gindex_history{r},sw_gindex(r));
        
        sw_mab_c_reward = vertcat(sw_mab_c_reward,sw_mab_c_reward(end)+data.RELAY_RESULT_MATLAB);
        sw_mab_c_reward_times = vertcat(sw_mab_c_reward_times,sw_mab_c_reward_times(end)+toc(tstart)-paused_time);
        
        sw_obs = sw_obs + 1;
        r_hist = r;
        
        [val,r] = max(sw_gindex);
        
        if r~=r_hist %have to make a decision
            if sw_gindex(r_hist) >= min(sw_gindex_history{r_hist}(1:end-1))
                r = r_hist; %keep playing this arm
                sw_mode_decisions = vertcat(sw_mode_decisions,-1);
                %brute force analysis
            else
                sw_mode_decisions = vertcat(sw_mode_decisions,1);
                
                %Initialize each path
                current_means = sw_current_means;
                policy_indices = zeros(9,1);
                cost = switching_matrix(r_hist,:);
                current_a = alphas;
                current_b = betas;
                
                %cost of switch
                policy_indices = policy_indices - cost';
                %predict reward
                policy_indices = policy_indices + current_means;
                %project index
                
                estart = tic;
                if horizon==1
                    for i=1:8
                        policy_indices = policy_indices + current_means(i)*interpolateBI(current_a(i)+1,current_b(i));
                    end
                else
                    for m=horizon:-1:2
                        if m==horizon
                            intermediate_indices = zeros(9^(m-1),1);
                            expanded_indices = zeros(9^m,1);
                        else
                            expanded_indices = intermediate_indices;
                            intermediate_indices = zeros(9^(m-1),1);
                        end
                        
                        beta_sum = 0;
                        for k=0:m
                            beta_sum = beta_sum + beta^k;
                        end
                        expanded_beta = beta^m / beta_sum;
                        
                        for j=9:9:9^m
                            if mod(j/9,9)~=0
                                cost = switching_matrix(mod(j/9,9),:);
                            else
                                cost =  switching_matrix(9,:);
                            end
                            expanded_indices(j-8:j) = expanded_indices(j-8:j)-cost'*expanded_beta;
                            current_means = sw_current_means;
                            expanded_indices(j-8:j) = expanded_indices(j-8:j) + current_means.^m*expanded_beta;
                            current_a = alphas;
                            current_b = betas;
                            if m==horizon
                                for i=1:9
                                    expanded_indices(j-8:j) =  expanded_indices(j-8:j) + current_means(i)^m*interpolateBI(current_a(i)+m,current_b(i));
                                end
                            end
                            if m~=2
                                intermediate_indices(j/9) = max(expanded_indices(j-8:j));
                            else
                                policy_indices(j/9) = policy_indices(j/9) + max(expanded_indices(j-8:j));
                            end
                        end
                    end
                end
                
                policy_indices(r_hist) = NaN;
                [val,r] = max(policy_indices);
                
                time = toc(estart);
                enum_times = [enum_times;time];
            end
        else
            sw_mode_decisions = [sw_mode_decisions;-2];
        end
        
        sw_r_decisions = vertcat(sw_r_decisions,r);
        iMatlab('MOOS_MAIL_TX','RELAY_WAYPOINT',[num2str(wp(r,1)) ',' num2str(wp(r,2))]);
        waiting = 1;
        disp(['Sent Waypoint ' num2str(r)])
        
        if showfigures
            stem(figure_handles(1),sw_mode_decisions,'r')
            stem(figure_handles(2),sw_r_decisions,'r')
            plot(figure_handles(3),sw_mab_c_reward./(1:length(sw_mab_c_reward))','-o','markersize',4,'linewidth',1.8);
            plot(figure_handles(4),sw_mab_c_reward_times/60,sw_mab_c_reward,'-o','markersize',4,'linewidth',1.8);
            
            for k=1:9
                hold(figure_handles(5),'on')
                plot(figure_handles(5),sw_mab_c_reward_by_point{k}./(1:length(sw_mab_c_reward_by_point{k}))','-o','markersize',4,'linewidth',1.8,'color',cc(k,:));
            end
            hold(figure_handles(5),'off')
        end
        
        save SCRelay.mat
        disp('Saved Data')
        
    end
end
