%function to create control commands in binary form for robot arm
clc
clear all
close all

s = serial('COM1')
set(s,'BaudRate',115200)
set(s,'ByteOrder','bigEndian')
get(s)

fopen(s)

% %messages to do nothing
%command3 =['E7';'FF';'FF';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'01';...
    %'00';'00';'00';'FF';'14';'00';'00';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'01';'00';'00';'00';'FF';'14';'00';'00';'00';'49';'E5'];
command0 =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';...
    '00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'34';'E5']));
command0(50) = mod(sum(command0(1:49)),255)-floor(sum(command0(1:49))/255);

% motor 1 position command
command1up =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'01';'FF';'FF';'0F';'FF';'0F';'FF';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';...
    '00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'E7';'17';'70';'0A';'01';'13';'0E';'00';'00';'40';'E5']));
command1up(50) = mod(sum(command1up(1:49)),255)-floor(sum(command1up(1:49))/255);

command1d =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'02';'FF';'FF';'0F';'FF';'0F';'FF';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';...
    '00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'E7';'17';'70';'0A';'01';'13';'0E';'00';'00';'40';'E5']));
command1d(50) = mod(sum(command1d(1:49)),255)-floor(sum(command1d(1:49))/255);


%motor 2 speed command
% command2 =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'04';'03';'E8';'0F';'FF';'0F';'FF';'00';'00';...
%     '00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'34';'E5']));
% command2(50) = mod(sum(command2(1:49)),255)-floor(sum(command2(1:49))/255);

%motor 2 position command
command2 =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'FF';'FF';'0F';'FF';'0F';'FF';'00';'00';...
'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'34';'E5']));
command2(50) = mod(sum(command2(1:49)),255)-floor(sum(command2(1:49))/255);

%motor 3 position command
command3 =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';...
    '02';'FF';'FF';'0F';'FF';'0F';'FF';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'34';'E5']));
command3(50) = mod(sum(command3(1:49)),255)-floor(sum(command3(1:49))/255);

%'00';'01';'FF';'FF';'0F';'FF';'0F';'FF'
%motor 4 position command
command4 =uint8(hex2dec(['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';...
    '00';'00';'00';'0F';'FF';'00';'32';'00';'00';'01';'FF';'FF';'0F';'FF';'0F';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'34';'E5']));
command4(50) = mod(sum(command4(1:49)),255)-floor(sum(command4(1:49))/255);

%motor 5 position command
command5 = uint8(hex2dec(['E7';'FF';'FF';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';...
    '32';'00';'00';'00';'00';'00';'0F';'FF';'00';'32';'00';'00';'02';'00';'01';'0F';'FF';'00';'32';'00';'00';'01';'FF';'FF';'0F';'FF';'0F';'FF';'00';'34';'E5']));
command5(50) = mod(sum(command5(1:49)),255)-floor(sum(command5(1:49))/255);

% command5 = uint8(hex2dec(command3));
% sum1 = sum(command4(1:49)); %check sum
% command4(50) = mod(sum1,255)-floor(sum1/255);

n=810;
dt=0.05;
out2 = zeros(51,n);
time = dt:dt:n*dt;
% i=1;
% while i<n+1
% fwrite(s,command2)
% pause(dt)
% out2(:,i) = fread(s,s.BytesAvailable,'uint8');
% 
% i=i+1;
% end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%retract from cup
% while i<350
%     if i<25
%        fwrite(s,command5);
%        pause(0.05)
%     end
%     
%     if i>=25 && i<150
%         fwrite(s,command1up);
%         pause(0.05)
%     end
% 
%       if i>=150 && i<275
%           fwrite(s,command3)
%           pause(0.05)
%       end
%     if i>=275 && i<400
%         fwrite(s,command2)
%         pause(0.05)
%     end
%     
%     i=i+1;
% end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%pick up cup, starting from zeroed position
i=1;
while i<810
    
     if i<200
      fwrite(s,command2)
      pause(0.05)
     end
        
     if i>=200 && i<400
        fwrite(s,command1up)
        pause(0.05)
     end
      
     if i>=400 && i<550
        fwrite(s,command3);
        pause(0.05)
     end
    
    if i>=550 && i<675
       fwrite(s,command1d);
       pause(0.05)
    end
    
if i>=675 && i<700
    fwrite(s,command5)
    pause(0.05)
end

if i>=700 && i<800
fwrite(s,command1up)
pause(0.05)
end 

if i>=800 && i<810
fwrite(s,command4)
pause(0.05)
end

out2(:,i) = fread(s,s.BytesAvailable,'uint8');
    i=i+1;
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%out2 = fread(s,s.BytesAvailable,'uint8');

figure
plot(time,360/255*out2(7,:),time,360/255*out2(16,:),time,360/255*out2(25,:),time,360/255*out2(34,:),time,360/255*out2(43,:),'LineWidth',3)
xlabel('Time(sec)','FontSize',16)
ylabel('Arm Angle [degrees]','FontSize',16);
h=legend('Motor 1','2','3','4','5')
grid on

set(gca,'FontSize',16)
rect = [0.5, 0.75, .1, .1];
set(h, 'Position', rect)
    
print(gcf, '-depsc', 'ArmPickupCup1')
print(gcf, '-dpng', 'ArmPickupCup1')

fclose(s)
delete(s)
clear s










