/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <sstream>
#include "acomms_driver.h"
#include <acomms_messages.h>

using namespace std;

//---------------------------------------------------------
// Constructor

acomms_driver::acomms_driver()
{
	driver_ready = false;
    driver_initialized = false;
    port_name = "/dev/ttyUSB0";
    my_name = "default_name";

	transmission_rate = 0;
	transmission_dest = 0;
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
      } else if ( key == "ACOMMS_TRANSMIT_DEST" ) {
    	  transmission_dest = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DATA" ) {
    	  transmission_data = msg.GetString();
    	  transmit_data( false );
      } else if ( key == "ACOMMS_TRANSMIT_DATA_BINARY" ) {
    	  transmission_data = msg.GetString();
    	  transmit_data( true );
      } else if ( key == "LOGGER_DIRECTORY" && !driver_initialized ) {
          my_name = msg.GetCommunity();
          startDriver( msg.GetString() );
      }
   }
	
   return(true);
}

void acomms_driver::RegisterVariables() {
	m_Comms.Register( "ACOMMS_TRANSMIT_RATE", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DEST", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA_BINARY", 0 );
    m_Comms.Register( "LOGGER_DIRECTORY", 0 );
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_driver::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   m_MissionReader.GetConfigurationParam("PortName", port_name);
   m_MissionReader.GetConfigurationParam("ID", my_id);
   // m_Comms.Register("VARNAME", is_float(int));

   RegisterVariables();

   publishStatus("not running");

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool acomms_driver::Iterate()
{
	driver->do_work();
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

	publishStatus("transmitting");
	driver_ready = false;

	goby::acomms::protobuf::ModemTransmission transmit_message;
	transmit_message.set_type(goby::acomms::protobuf::ModemTransmission::DATA);
	transmit_message.set_src(goby::util::as<unsigned>(my_id));
	if ( transmission_dest == 0 )
		transmit_message.set_dest(goby::acomms::BROADCAST_ID);
	else
		transmit_message.set_dest( transmission_dest );
	transmit_message.set_rate( transmission_rate );

	if ( transmission_rate == 0 ) {
		int data_size = transmission_data.size();
		if ( data_size > 32 ) {
			transmit_message.add_frame( transmission_data.data(), 32 );
		} else {
			transmit_message.add_frame( transmission_data.data(), data_size );
		}
		transmission_data = "";
	} else if ( transmission_rate == 2 ) {
		for ( int i=0; i<3; i++ ) {
			int data_size = transmission_data.size();
			if ( data_size > 64 ) {
				transmit_message.add_frame( transmission_data.data(), 64 );
				transmission_data = transmission_data.substr( 64, data_size-64 );
			} else if ( data_size > 0 ) {
				transmit_message.add_frame( transmission_data.data(), data_size);
				break;
			}
		}
		transmission_data = "";
	}

	transmit_message.set_ack_requested(false);

	driver->handle_initiate_transmission( transmit_message );

    lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info;
    transmit_info.vehicle_name = my_name;
    transmit_info.rate = transmit_message.rate();
    transmit_info.dest = transmit_message.dest();
    transmit_info.num_frames = transmit_message.frame_size();

    m_Comms.Notify("ACOMMS_TRANSMIT_SIMPLE", transmit_info.serializeToString());
}

void acomms_driver::handle_data_receive( const goby::acomms::protobuf::ModemTransmission& data_msg ) {
	string publish_me = data_msg.DebugString();
    while ( publish_me.find("\n") != string::npos ) {
            publish_me.replace( publish_me.find("\n"), 1, "<|>" );
    }
    m_Comms.Notify("ACOMMS_RECEIVED_ALL", publish_me);

    int num_stats = data_msg.ExtensionSize( micromodem::protobuf::receive_stat );
    if ( num_stats == 1 ) {
            publishReceivedInfo( data_msg, 0 );
    } else if ( num_stats == 2 ) {
            publishReceivedInfo( data_msg, 1 );
    } else {
            stringstream ss;
            ss << "Error handling ModemTransmission - contained " << num_stats << "statistics.";
            publishWarning( ss.str() );
    }
}

void acomms_driver::handle_raw_incoming( const goby::acomms::protobuf::ModemRaw& msg ) {
	string descriptor = msg.raw().substr(3,3);
	if ( descriptor == "TXF") {
//		cout << "transmission finished" << endl;
		driver_ready = true;
		publishStatus("ready");
	} else if ( descriptor == "RXP" ) {
		driver_ready = false;
		CST_received = false;
		RXD_received = false;
		publishStatus("receiving");
	} else if ( descriptor == "CST" ) {
		CST_received = true;
	} else if ( descriptor == "RXD" ) {
		RXD_received = true;
	}
//	} else {
//		cout << "unhandled raw msg with descriptor " << descriptor << endl;
//	}
	if ( !driver_ready && CST_received && RXD_received ) {
		CST_received = false;
		RXD_received = false;
		driver_ready = true;
		publishStatus("ready");
	}
}

void acomms_driver::publishReceivedInfo( goby::acomms::protobuf::ModemTransmission trans, int index ) {
        micromodem::protobuf::ReceiveStatistics stat = trans.GetExtension( micromodem::protobuf::receive_stat, index );

        lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
        receive_info.vehicle_name = my_name;
        receive_info.rate = stat.rate();
        receive_info.num_frames = stat.number_frames();
        receive_info.num_bad_frames = stat.number_bad_frames();
        receive_info.num_good_frames = receive_info.num_frames - receive_info.num_bad_frames;
        m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());

        m_Comms.Notify("ACOMMS_RECEIVED_SNR_IN", stat.snr_in());
}

void acomms_driver::publishWarning( std::string message ) {
	m_Comms.Notify("ACOMMS_DRIVER_WARNING", message );
}

void acomms_driver::publishStatus( std::string status ) {
	m_Comms.Notify("ACOMMS_DRIVER_STATUS", status );
}

void acomms_driver::startDriver( std::string logDirectory ) {
	cout << "opening log file: " << logDirectory+"/goby_log.txt" << endl;
	verbose_log.open((logDirectory+"/goby_log.txt").c_str());

	cfg.set_serial_port( port_name );
	cfg.set_modem_id( my_id );

	goby::glog.set_name( "iacomms_driver" );
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
	goby::acomms::connect( &driver->signal_raw_incoming, boost::bind(&acomms_driver::handle_raw_incoming, this, _1) );

	driver->startup(cfg);
	driver_ready = true;
	driver_initialized = true;

	publishStatus("ready");
}
