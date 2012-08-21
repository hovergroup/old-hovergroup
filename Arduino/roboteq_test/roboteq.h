#ifndef Roboteq_H
#define Roboteq_H

#include <Arduino.h>

class Roboteq
{
public:
  Roboteq(Stream& port);
  
  boolean doWork();
  
  void initialize();
  
  // runtime commands
  void setEmergencyStop();
  void resetEmergencyStop();
  void setOutput( int output );
  void setAccleration( int acceleration );
  void setDeceleration( int deceleration );
  
  // runtime queries
  float readMotorAmps();
  float readBatteryAmps();
  boolean readSingleDigitalInput( int input_number );
  
  float getBatteryVoltage() { return battery_voltage; }
  int getHeatsinkTemp() { return heatsink_temp; }
  
  boolean getOverheatFault() { return overheat; }
  boolean getOvervoltageFault() { return overvoltage; }
  boolean getUndervoltageFault() { return undervoltage; }
  boolean getShortCircuitFault() { return short_circuit; }
  boolean getEmergencyStopFault() { return emergency_stop; }
  boolean getSepexExcitationFault() { return sepex_excitation_fault; }
  boolean getMosfetFailureFault() { return mosfet_failure; }
  boolean getStartupConfigurationFault() { return startup_configuration_fault; }
  char* getFaultSummary();
  
  boolean getSerialModeStatus() { return serial_mode; }
  boolean getPulseModeStatus() { return pulse_mode; }
  boolean getAnalogModeStatus() { return analog_mode; }
  boolean getPowerStageOffStatus() { return power_stage_off; }
  boolean getStallDetectedStatus() { return stall_detected; }
  boolean getAtLimitStatus() { return at_limit; }
  boolean getScriptRunningStatus() { return script_running; }
  char* getStatusSummary();
  
  void setPower( int velocity );
  
private:
  Stream& _port;
  static const unsigned int ROBOTEQ_TIMEOUT = 50; // timeout in ms when reading line
  static const unsigned int ROBOTEQ_READ_UPDATE = 50;
  unsigned long last_read_time;
  int read_state;
  
  int current_velocity;
  
  char summary_string [200];
  
  boolean overheat, overvoltage, undervoltage, short_circuit, 
    emergency_stop, sepex_excitation_fault, mosfet_failure, 
    startup_configuration_fault;
  boolean serial_mode, pulse_mode, analog_mode, power_stage_off, 
    stall_detected, at_limit, script_running;
  int internal_temp, heatsink_temp;
  float internal_voltage, battery_voltage, five_voltage;
  
  char buffer[100];
  int buffer_index;
  int readLine();
  
  void readBuffer();
  int processBuffer();
  int findLine( int index );
  void shiftBuffer( int shift );
  
  void parseVoltage( int index, int stopIndex );
  void parseTemperature( int index, int stopIndex );
  
  void sendSpeed( int velocity );
  
  // runtime updates
  boolean updateFaultFlags();
  boolean updateStatusFlags();
};

#endif
