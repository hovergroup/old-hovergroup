/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "acomms_driver.h"

using namespace std;

//---------------------------------------------------------
// Constructor

acomms_driver::acomms_driver()
{
	driver_ready = false;
	port_name = "/dev/ttyUSB0";
}

//---------------------------------------------------------
// Destructor

acomms_driver::~acomms_driver()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool acomms_driver::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_driver::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   m_MissionReader.GetConfigurationParam("PORT_NAME", port_name);
   m_MissionReader.GetConfigurationParam("ID", my_id);
   // m_Comms.Register("VARNAME", is_float(int));

   m_Comms.Notify()
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool acomms_driver::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool acomms_driver::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}


void acomms_driver::handle_data_receive( const goby::acomms::protobuf::ModemTransmission& data_msg ) {

}

void acomms_driver::startDriver( std::string logDirectory ) {
	cout << "opening log file: " << logDirectory+"/goby_log.txt" << endl;
	verbose_log.open((logDirectory+"/goby_log.txt").c_str());

	cfg.set_serial_port( port_name );
	cfg.set_modem_id( my_id );

	goby::glog.set_name( "driver_switch_r1" );
	goby::glog.add_stream( goby::common::logger::DEBUG1, &std::clog );
	goby::glog.add_stream( goby::common::logger::DEBUG3, &verbose_log );

	std::cout << "Starting WHOI Micro-Modem MMDriver" << std::endl;
	driver = new goby::acomms::MMDriver;
	// turn data quality factor message on
	// (example of setting NVRAM configuration)
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DQF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MFD,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "SHF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DOP,1");

	goby::acomms::connect( &driver->signal_receive, boost::bind(&acomms_driver::handle_data_receive, this, _1) );

	driver->startup(cfg);
	driver_ready = true;
}
