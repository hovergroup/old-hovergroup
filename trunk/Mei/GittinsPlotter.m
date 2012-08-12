
% Gittins Plotter

wp = importdata('relay_waypoints.txt');

fileToRead1 = 'icarus_kassandra_logfile.txt'];
newData1 = importdata(fileToRead1);
% Create new variables in the base workspace from those fields.
for i = 1:size(newData1.colheaders, 2)
    assignin('base', genvarname(newData1.colheaders{i}), newData1.data(:,i));
end

fileToRead2 = 'kassandra_nostromo_logfile.txt'];
newData2 = importdata(fileToRead2);
% Create new variables in the base workspace from those fields.
for i = 1:size(newData2.colheaders, 2)
    assignin('base', genvarname(newData2.colheaders{i}), newData2.data(:,i));
end

fid = fopen('output.txt');
times = textscan(fid,'%f %*[^\n]');
fclose(fid);
times = times{1};

fid = fopen('output.txt');
stats = textscan(fid,'%*53c %s %*9c %f %*9c %f %*11c %f %*11c %f %*8c %f %*8c %f %*8c %f %*[^\n]', 'Delimiter', '<|>');
strings = stats{1}; 
myx = stats{2};
myy = stats{3};
nextx = stats{4};
nexty = stats{5};
mean = stats{6};
var = stats{7};
index= stats{8};
fclose(fid);

% Plot the final packet loss rates

