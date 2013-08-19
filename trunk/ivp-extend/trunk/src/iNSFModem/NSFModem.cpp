/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: NSFModem.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "NSFModem.h"

using namespace std;

//---------------------------------------------------------
// Constructor

NSFModem::NSFModem() : m_state(Starting),
                       m_power_increase_pin_value(c_gpio5_val.c_str()),
                       m_power_increase_pin_direction(c_gpio5_dir.c_str()),
                       m_power_decrease_pin_value(c_gpio6_val.c_str()),
                       m_power_decrease_pin_direction(c_gpio6_dir.c_str())
{

  if (!m_power_increase_pin_direction.is_open())
  {
    std::cerr << "Unable to open TX power increase pin (direction)\n";
  }
  if (!m_power_increase_pin_value.is_open())
  {
    std::cerr << "Unable to open TX power increase pin (value)\n";
  }
  if (!m_power_decrease_pin_direction.is_open())
  {
    std::cerr << "Unable to open TX power decrease pin (direction)\n";
  }
  if (!m_power_decrease_pin_value.is_open())
  {
    std::cerr << "Unable to open TX power decrease pin (value)\n";
  }


  m_power_increase_pin_direction << "out";
  m_power_decrease_pin_direction << "out";

  // set tx power to minimum
  for (unsigned short int i = 0; i < 32; i++)
  {
    m_power_decrease_pin_value << "0"; // set GPIO6 to LOW
    boost::this_thread::sleep( boost::posix_time::milliseconds(30) ); // sleep
    m_power_decrease_pin_value << "1";// set GPIO6 to HIGH
  }

  m_requested_power_level = 0;
  m_current_power_level = 0;

  print_power_status();
}

//---------------------------------------------------------
// Destructor

NSFModem::~NSFModem()
{
  // release pins
  m_power_increase_pin_value.close();
  m_power_decrease_pin_direction.close();
  m_power_decrease_pin_value.close();
  m_power_decrease_pin_direction.close();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool NSFModem::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) 
  {
    CMOOSMsg &msg = *p;

    std::string key = msg.GetKey();

    if( key == "NSFMODEM_POWER_LEVEL")
    {
      m_requested_power_level = static_cast<int>(msg.GetDouble());
      if ( m_requested_power_level > 31)
      {
        m_requested_power_level = 31;
        std::cout << "Limiting TX power level to 31.\n";
      }
      else if ( m_requested_power_level < 0)
      {
        m_requested_power_level = 0;
        std::cout << "Limiting TX power level to 0.\n";
      }
      std::cout << "TX power: " << m_requested_power_level << "/" << m_current_power_level << " (requested/current)\n";
    }


    // update the requested power level

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool NSFModem::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", 0);
	
  m_state = Running;

  RegisterVariables();

  m_power_write_thread = boost::thread(boost::bind(&NSFModem::power_write_loop, this));

  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool NSFModem::Iterate()
{
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool NSFModem::OnStartUp()
{
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void NSFModem::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("NSFMODEM_POWER_LEVEL", 0);
}

void NSFModem::power_write_loop()
{
  while(m_state == Running)
  {
    // check current power level wrt latest request
    // determine delta (= request - current)
    int delta = m_requested_power_level - m_current_power_level;

    if ( delta > 0)
    {
      // power needs to be increased (delta > 0)
      std::cout << "Increasing TX power.\n";
      m_power_increase_pin_value << 0;  // set GPIO5 to LOW
      boost::this_thread::sleep( boost::posix_time::milliseconds(30) ); // sleep
      m_power_increase_pin_value << 1; // set GPIO5 to HIGH
      m_current_power_level++;
      print_power_status();
    }
    else if ( delta < 0)
    {
      // power needs to be decreased (delta < 0)
      std::cout << "Decreasing TX power.\n";
      m_power_decrease_pin_value << 0; // set GPIO6 to LOW
      boost::this_thread::sleep( boost::posix_time::milliseconds(30) ); // sleep
      m_power_decrease_pin_value << 1;// set GPIO6 to HIGH
      m_current_power_level--;
      print_power_status();
    }
    else
    {
      boost::this_thread::sleep( boost::posix_time::milliseconds(200) );
      /*
      m_power_increase_pin_direction.flush();
      m_power_increase_pin_value.flush();
      m_power_decrease_pin_direction.flush();
      m_power_decrease_pin_value.flush();
      */
    }
  }
}

void NSFModem::print_power_status()  
{
  std::cout << "TX power: current=" << m_current_power_level << ", requested=" << m_requested_power_level << "\n";
}
