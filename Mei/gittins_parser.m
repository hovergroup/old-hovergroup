clc; close all; clear all

fid = fopen('gittins_stats_2.txt');
stats = textscan(fid,'%f %*2c %s %*2c %f %*2c %f %*2c %d %*2c %f %*2c %f %*2c %f %*2c %f %*2c %f %*[^\n]', 'Delimiter', '<|>');
times = stats{1};
times_scaled = times/60;
strings = stats{2};
myx = stats{3};
myy = stats{4};
nexti = stats{5}+1;
mean = stats{6};
var = stats{7};
stat_obs = stats{8};
index = stats{9};
successes = stats{10};
fclose(fid);

wp = importdata('relay_waypoints.txt');

fileToRead1 = 'i2k_logfile.txt';
newData1 = importdata(fileToRead1);

for i = 1:size(newData1.colheaders, 2)
    assignin('base', genvarname(newData1.colheaders{i}), newData1.data(:,i));
end

fileToRead1 = 'k2n_logfile.txt';
newData1 = importdata(fileToRead1);

for i = 1:size(newData1.colheaders, 2)
    assignin('base', genvarname(newData1.colheaders{i}), newData1.data(:,i));
end

%% %---------------------------------------------------------------------------

obs = length(times_scaled);
for i=1:9
    plots{i}=NaN;
    plotsindex{i}=NaN;
    plotsvar{i}=NaN;
    stimes{i}=0;
    ctimes{i}=times_scaled(1);
    plotx{i}=0;
    ploty{i}=0;
end

lines = [0 0];
lines3 = [];
linesindex = [0 0];
linesvar = [0 0];
linecolor = 1;
linesctime = [times_scaled(1) 0];

for i=1:obs
    if strcmp(strings(i),'Decision')
        ttime = stimes{nexti(i-1)}(end);
    elseif i<18
        ttime = i+1;
    else
        ttime = 19+(i-19)/2;
    end
    
    plots{nexti(i)}=vertcat(plots{nexti(i)},max(mean(i),0));
    plotsvar{nexti(i)}=vertcat(plotsvar{nexti(i)},var(i));
    plotx{nexti(i)}=vertcat(plotx{nexti(i)},myx(i));
    ploty{nexti(i)}=vertcat(ploty{nexti(i)},myy(i));
    plotsindex{nexti(i)}=vertcat(plotsindex{nexti(i)},max(index(i),0));
    stimes{nexti(i)}=vertcat(stimes{nexti(i)},ttime);
    ctimes{nexti(i)}=vertcat(ctimes{nexti(i)},times_scaled(i));
    
    lines = vertcat(lines,[ttime max(mean(i),0)]);
    linesvar = vertcat(linesvar,[ttime var(i)]);
    linesindex = vertcat(linesindex,[ttime max(index(i),0)]);
    lines3 = vertcat(lines3,[myx(i) myy(i) max(mean(i),0)]);
    linecolor = vertcat(linecolor, nexti(i));
    linesctime = vertcat(linesctime,[times_scaled(i) max(mean(i),0)]);
end

for i=1:9
    plots{i}=vertcat(plots{i},plots{i}(end));
    plotsvar{i}=vertcat(plotsvar{i},plotsvar{i}(end));
    plotsindex{i}=vertcat(plotsindex{i},plotsindex{i}(end));
    stimes{i}=vertcat(stimes{i},55);
    ctimes{i}=vertcat(ctimes{i},times_scaled(end));
end

cc = hsv(9);

figure;
for i=1:9
    subplot(3,1,2),line(stimes{i},plots{i},'color',cc(i,:),'linestyle','-','linewidth',1.5);
    hold all
    subplot(3,1,2),stem(stimes{i},plots{i},'fill','color',cc(i,:),'linestyle','--');
end
patch([0 0 18 18],[0 1 1 0],[0.8 0.8 0.8])

for i=1:length(lines)-1
    if i<2
        patch([lines(i,1) lines(i,1) lines(i+1,1) lines(i+1,1)],[0 lines(i,2) lines(i+1,2) 0],cc(linecolor(i),:),'FaceAlpha',0.5,'EdgeColor','none')
        text((lines(i,1)+lines(i+1,1))/2,(lines(i+1,2)+lines(i,2))/4,num2str(linecolor(i)))
    elseif mod(i,2)~=0
        patch([lines(i,1) lines(i,1) lines(i+1,1) lines(i+1,1)],[0 lines(i,2) lines(i+1,2) 0],cc(linecolor(i),:),'FaceAlpha',0.5,'EdgeColor','none')
        text((linesindex(i,1)+linesindex(i+1,1))/2,(lines(i,2)+lines(i+1,2))/4,num2str(linecolor(i)))
    end
end

for i=1:9
    subplot(3,1,1),line(stimes{i},plotsindex{i},'color',cc(i,:),'linestyle','-','linewidth',1.5);
    %set(gca,'YScale','log')
    hold all
    subplot(3,1,1),stem(stimes{i},plotsindex{i},'linestyle','--','color',cc(i,:),'marker','o','markerfacecolor',cc(i,:));
end
patch([0 0 18 18],[0 6 6 0],[0.8 0.8 0.8])


for i=1:length(linesindex)-1
    if i<2
        patch([linesindex(i,1) linesindex(i,1) linesindex(i+1,1) linesindex(i+1,1)],[0 linesindex(i,2) linesindex(i+1,2) 0],cc(linecolor(i+1),:),'FaceAlpha',0.5,'EdgeColor','none')
        text((linesindex(i,1)+linesindex(i+1,1))/2,(linesindex(i+1,2)+linesindex(i,2))/4,num2str(linecolor(i+1)))
    elseif mod(i,2)~=0
        patch([linesindex(i,1) linesindex(i,1) linesindex(i+1,1) linesindex(i+1,1)],[0 linesindex(i,2) linesindex(i+1,2) 0],cc(linecolor(i+1),:),'FaceAlpha',0.5,'EdgeColor','none')
        text((linesindex(i,1)+linesindex(i+1,1))/2,(linesindex(i,2)+linesindex(i+1,2))/4,num2str(linecolor(i)))
    end
end

for i=1:9
    subplot(3,1,3),line(stimes{i},plotsvar{i},'color',cc(i,:),'linestyle','-','linewidth',1.2);
    hold all
    subplot(3,1,3),stem(stimes{i},plotsvar{i},'color',cc(i,:),'linestyle','--','marker','o','markerfacecolor',cc(i,:));
    hold all
end
patch([0 0 18 18],[0 0.8 0.8 0],[0.8 0.8 0.8])

for i=1:length(linesvar)-1
    if i<2
        patch([linesvar(i,1) linesvar(i,1) linesvar(i+1,1) linesvar(i+1,1)],[0 linesvar(i,2) linesvar(i+1,2) 0],cc(linecolor(i+1),:),'FaceAlpha',0.5,'EdgeColor','none')
        text((linesvar(i,1)+linesvar(i+1,1))/2,(linesvar(i+1,2)+linesvar(i,2))/4,num2str(linecolor(i+1)))
    elseif mod(i,2)~=0
        patch([linesvar(i,1) linesvar(i,1) linesvar(i+1,1) linesvar(i+1,1)],[0 linesvar(i,2) linesvar(i+1,2) 0],cc(linecolor(i+1),:),'FaceAlpha',0.5,'EdgeColor','none ')
        text((linesvar(i,1)+linesvar(i+1,1))/2,(linesvar(i+1,2)+linesvar(i,2))/4,num2str(linecolor(i+1)))
        line([linesvar(i,1);linesvar(i+1,1)],[linesvar(i,2);linesvar(i+1,2)],'color',cc(linecolor(i+1),:))
    end
end

%% %-----------------------------------------------

clear total_reward_by_point total_reward

index_offset = 1;
index_end = 730;
time_offset = 39441.8;

% index_offset = 733;
% index_end = size(transmission_time_i2k,1);

length = 192;
link_counter=1;
ik_counter=1;
kn_counter=1;
total_reward=0;
current_counter=1;

for i=1:9
    total_reward_by_point{i} = 1.5;
end

for i=1:size(times,1)
    % Find nearest waypoint- possibly miss some due to waypoint similarity
    %     myx = rgps_x_i2k(i);
    %     myy = rgps_y_i2k(i);
    %
    %     tempx = abs(wp(:,1) - myx);
    %     tempy = abs(wp(:,2) - myy);
    %
    %     tempxx = find(tempx==min(tempx));
    %     tempyy = find(tempy==min(tempy));
    %     myind = tempxx(find(ismember(tempxx,tempyy)));
    
    if strcmp(strings(i),'Current')
        
        current_time = times(i);
        current_ind = nexti(i);
        
        if i<18
        total_reward_by_point{current_ind} = vertcat(total_reward_by_point{current_ind},length*successes(i)/2);
        total_reward_by_point{current_ind} = vertcat(total_reward_by_point{current_ind},length*successes(i));
        
        else
        total_reward_by_point{current_ind} = vertcat(total_reward_by_point{current_ind},length*successes(i));
        total_reward = vertcat(total_reward, length*successes(i));
        end
        
        for j=1:9
            if j~=current_ind
                if i<18
                total_reward_by_point{j} = vertcat(total_reward_by_point{j},total_reward_by_point{j}(end));
                total_reward_by_point{j} = vertcat(total_reward_by_point{j},total_reward_by_point{j}(end));
                else
                total_reward_by_point{j} = vertcat(total_reward_by_point{j},total_reward_by_point{j}(end));
                end
            end
        end
%                 disp(current_counter);
%                 current_counter=1;
%         
%                 while receive_time_i2k(ik_counter) <= current_time
%         
%                     if receive_status_i2k(ik_counter)==0
%                         if receive_status_k2n(link_counter)==0
%                             reward_now = length;
%                         else
%                             reward_now = 0;
%                         end
%                         link_counter = link_counter+1;
%                     else
%                         reward_now = 0;
%                     end
%         
%               %      if ik_counter < 160 || ik_counter > 164
%                     if receiver_thrust_i2k(ik_counter)==0
%                                     current_counter=current_counter+1;
%                             total_reward = vertcat(total_reward,total_reward(end)+reward_now);
%                             total_reward_by_point{current_ind} = vertcat(total_reward_by_point{current_ind},total_reward_by_point{current_ind}(end)+reward_now);
%                             for j=1:9
%                                if j~=current_ind
%                                     total_reward_by_point{j} = vertcat(total_reward_by_point{j},total_reward_by_point{j}(end));
%                                 end
%                             end
%                %     end
%                     end
%                     ik_counter=ik_counter+1;
%                 end %end while

    end %end if
end %end time loop

%remove intialization
for i=1:9
    total_reward_by_point{i}= total_reward_by_point{i}(2:end);
end

total_reward = total_reward_by_point{1};
for i=2:9
    total_reward = total_reward + +total_reward_by_point{i};
end

figure

for i=1:9
    subplot(2,1,2), plot(0:1:size(total_reward_by_point{i},1)-1,total_reward_by_point{i},'color',cc(i,:),'linewidth',1.5);
    hold all
end
subplot(2,1,2), patch([0 0 18 18],[0 40000 40000 0],[0.8 0.8 0.8],'FaceAlpha',0.5)

subplot(2,1,1), plot(0:1:size(total_reward,1)-1,total_reward,'k','linewidth',1.5);
hold on

%fit round robin
first_pass_index = 18;
fitx = 1:1:first_pass_index;
[p,s] = polyfit(fitx',total_reward(1:first_pass_index),1);
fitx = 1:1:size(total_reward,1);
Y = polyval(p,fitx);
subplot(2,1,1),plot(0:1:size(Y,2)-1,Y,'--r','linewidth',1.5);
subplot(2,1,1), patch([0 0 18 18],[0 60000 60000 0],[0.8 0.8 0.8], 'FaceAlpha',0.5)

hold on