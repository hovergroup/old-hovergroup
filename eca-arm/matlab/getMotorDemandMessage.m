function message = getMotorDemandMessage(demand_type, demand, speed_limit, current_limit)

	message = zeros(1, 9);

	% demand type
	% 0 = hold	
	% 1 = voltage (%PWM), CW  [16bit, 0-100]
	% 2 = voltage (%PWM), CCW [16bit, 0-100]
	% 3 = speed (rpm), CW [12bit, 0-4095]
	% 4 = speed (rpm), CCW [12bit, 0-4095]
	% 5 = position [16bit]
  if (false == isingteger(demand_type))
    message(2) = 0;
  else
    message(2) = demand_type;
  end

  % demand
  d = mod(demand, 256);
  message(3) = demand - d;  % MSB 
  message(4) = d;           % LSB 
  
  speed_limit = min(4095, speed_limit);
  s = mod(speed_limit, 256);
  % speed limit
  message(5) = speed_limit - s; % MSB
  message(6) = s;               % LSB
  
  % current limit
  current_limit = min(4095, current_limit);
  c = mod(current_limit, 256);
  message(7) = current_limit - c; % MSB
  message(8) = c;                 % LSB

  message = uint8(message);
end