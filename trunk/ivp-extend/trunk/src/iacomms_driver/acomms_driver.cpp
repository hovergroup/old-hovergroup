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
      string key = msg.GetKey();

      if ( key == "ACOMMS_TRANSMIT_RATE" ) {
    	  transmission_rate = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DESTINATION" ) {
    	  transmission_dest = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DATA" ) {
    	  transmission_data = msg.GetString();
    	  transmit_data( false );
      } else if ( key == "ACOMMS_TRANSMIT_DATA_BINARY" ) {
    	  transmission_data = msg.GetString();
    	  transmit_data( true );
      }
   }
	
   return(true);
}

void acomms_driver::RegisterVariables() {
	m_Comms.Register( "ACOMMS_TRANSMIT_RATE", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DEST", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA_BINARY", 0 );
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

   RegisterVariables();

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

void acomms_driver::transmit_data( bool isBinary ) {
	if ( !driver_ready ) {
		publishWarning("Driver not ready");
		return;
	}
	goby::acomms::protobuf::ModemTransmission transmit_message;
	transmit_message.set_type(goby::acomms::protobuf::ModemTransmission::DATA);
	transmit_message.set_src(goby::util::as<unsigned>(my_id));
	if ( transmission_dest == -1 )
		transmit_message.set_dest(goby::acomms::BROADCAST_ID);
	else
		transmit_message.set_dest( transmission_dest );
	transmit_message.set_rate( transmission_rate );

	if ( transmission_rate == 0 ) {
		int data_size = transmission_data.size();
		if ( data_size > 32 ) {
			transmit_message.add_frame( transmission_data.data(), 32 );
			transmission_data = transmission_data.substr( 32, data_size-32 );
		} else {
			transmit_message.add_frame( transmission_data.data(), data_size );
			transmission_data = "";
		}
	}

	transmit_message.set_ack_requested(false);

	driver->handle_initiate_transmission( transmit_message );


}

void acomms_driver::handle_data_receive( const goby::acomms::protobuf::ModemTransmission& data_msg ) {
	string publish_me = data_msg.DebugString();
	while ( publish_me.find("\n") != string::npos ) {
		publish_me.replace( publish_me.find("\n"), 1, "<|>" );
	}

	m_Comms.Notify("ACOMMS_RECEIVED_ALL", publish_me);
}

void acomms_driver::publishWarning( std::string message ) {
	m_Comms.Notify("ACOMMS_WARNING", message );
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
