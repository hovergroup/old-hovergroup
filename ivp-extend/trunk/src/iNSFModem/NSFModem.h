/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: NSFModem.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef NSFModem_HEADER
#define NSFModem_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

const std::string c_gpio5_dir = "/gpio/boardio5/direction";
const std::string c_gpio5_val = "/gpio/boardio5/value";
const std::string c_gpio6_dir = "/gpio/boardio6/direction";
const std::string c_gpio6_val = "/gpio/boardio6/value";

// testing purposes only
/*
const std::string c_gpio5_dir = "/home/pvt/tmp/gpio5direction.txt";
const std::string c_gpio5_val = "/home/pvt/tmp/gpio5value.txt";
const std::string c_gpio6_dir = "/home/pvt/tmp/gpio6direction.txt";
const std::string c_gpio6_val = "/home/pvt/tmp/gpio6value.txt";
*/
enum NSFModemState { Starting, Stopping, Running };

class NSFModem : public CMOOSApp
{
  public:
    NSFModem();
    ~NSFModem();

  protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();

  private:
    NSFModemState m_state;
    int m_current_power_level;    // the current power level
    int m_requested_power_level;  // the latest power level request

    std::ofstream m_power_increase_pin_value;
    std::ofstream m_power_increase_pin_direction;
    std::ofstream m_power_decrease_pin_value;
    std::ofstream m_power_decrease_pin_direction;

    boost::thread m_power_write_thread;

  private:
    void power_write_loop();

    void print_power_status();
};

#endif 
