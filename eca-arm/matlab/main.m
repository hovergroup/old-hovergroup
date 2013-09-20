clc
clear all

s = serial('COM1')
set(s,'BaudRate',115200)
set(s,'ByteOrder','bigEndian')
get(s)

fopen(s)

while(i < 1200)
    
  % 1 - actuate
  demand_type = [0 0 0 0 0];
  demand = [0 0 0 0 0];
  speed_limit = 4095*ones(1,5);
  current_limit = 4095*ones(1,5);
  
  command = getMotorDemandCommand(demand_type, demand, speed_limit, current_limit);

  if i<500
      fwrite(s, command);
  end

  % wait
  pause(0.05)

  % 2 - sense
  if 0 < s.BytesAvailable
    data = [data, fread(s,s.BytesAvailable,'uint8')];
    
    data_f = flipud(data);
    
    idx = find(data == hex2dec('E7'),1);
    data(1:idx-1)=[];
    data_t = data(1:51);

    if('E5'==dec2hex(data_t(51)))
        readMotorCommand(data_t);
    end
  
  end

  % 3 control
  %lolno
  
  i=i+1;
 end

pause(0.1)
get(s)

fclose(s)
delete(s)
clear s
