
% Gittins Plotter

figure
clear deriv ticks tticks reward
wp = importdata('relay_waypoints.txt');

fileToRead1 = 'icarus_kassandra_logfile.txt';
newData1 = importdata(fileToRead1);

for i = 1:size(newData1.colheaders, 2)
    assignin('base', genvarname(newData1.colheaders{i}), newData1.data(:,i));
end

fileToRead1 = 'kassandra_nostromo_logfile.txt';
newData1 = importdata(fileToRead1);

for i = 1:size(newData1.colheaders, 2)
    assignin('base', genvarname(newData1.colheaders{i}), newData1.data(:,i));
end

length = 32;
time_offset = 41775.2;
save_times = save_times + time_offset;
index_offset = 85;

    reward(1)=0;
    r(1)=0;

for i=index_offset:size(receive_status_i2k,1)
    if rate_i2k(i)==0
    if receive_status_i2k(i)==0 && receiver_thrust_i2k(i)==0
        
        [bla my_index] = min(abs(transmission_time_k2n-receive_time_i2k(i)));
        if receive_status_k2n(my_index)==0 && receiver_thrust_k2n(my_index)==0
            reward(i)=reward(end)+length;
            r(i) = length;
        else
            reward(i) = reward(end);
            r(i)=0;
        end
    else
        reward(i)=reward(end);
        r(i)=0;
    end
    end
end
tticks = (transmission_time_i2k)/60-transmission_time_i2k(80)/60;
plot(tticks(index_offset:end),reward(index_offset:end),'k','linewidth',2)
axis([0 400 0 10000])
hold on

time_averaged = 0;
last_time = transmission_time_i2k(1);
averaging = 300;
rsofar = r(1);
ticks=NaN;
deriv=0;

for i=2:size(transmission_time_i2k,1)
    time_averaged = time_averaged + transmission_time_i2k(i) - last_time;
    
    if time_averaged < averaging
        last_time = transmission_time_i2k(i);
        rsofar = rsofar + r(i);
    else
        deriv = vertcat(deriv,rsofar/time_averaged);
       
        time_averaged = time_averaged-averaging;
        rsofar = r(i);
        last_time = transmission_time_i2k(i);
        ticks = vertcat(ticks,last_time);
    end
end

%h2 = axes;
%stairs(h2,ticks/60,deriv*60)
%set(h2,'YAxisLocation','right')
%set(h2,'Color','none')
%set(h2,'FontSize',16)