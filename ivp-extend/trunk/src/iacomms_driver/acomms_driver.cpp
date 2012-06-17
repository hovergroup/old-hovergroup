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

	receive_set_time = 0;
	status_set_time = 0;
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
   int transmit_code = 0;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      string key = msg.GetKey();

      if ( key == "ACOMMS_TRANSMIT_RATE" ) {
    	  transmission_rate = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DEST" ) {
    	  transmission_dest = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DATA" ) {
    	  transmission_data = msg.GetString();
    	  transmit_code = 1;
//    	  transmit_data( false );
      } else if ( key == "ACOMMS_TRANSMIT_DATA_BINARY" ) {
    	  transmission_data = msg.GetString();
    	  transmit_code = 2;
//    	  transmit_data( true );
      } else if ( key == "LOGGER_DIRECTORY" && !driver_initialized ) {
//          my_name = msg.GetCommunity();
          startDriver( msg.GetString() );
      }
   }



   switch ( transmit_code ) {
   case 1:
	   transmit_data(false);
	   break;
   case 2:
	   transmit_data(true);
	   break;
   default:
	   break;
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
   m_MissionReader.GetValue("Community", my_name);
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
	if ( status=="receiving" && MOOSTime()-receive_set_time > 8 ) {
		publishWarning("Timed out in receiving state.");
		driver_ready = true;
		publishStatus("ready");
	}

	if ( MOOSTime()-status_set_time > 5 ) {
		publishStatus( status );
	}

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
	if ( transmission_rate == 100 ) {
		// send mini data transmission
		transmit_message.set_type(goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC);
		transmit_message.SetExtension( micromodem::protobuf::type, micromodem::protobuf::MICROMODEM_MINI_DATA );
	} else {
		// send normal fsk or psk
		transmit_message.set_type(goby::acomms::protobuf::ModemTransmission::DATA);
		transmit_message.set_rate( transmission_rate );
	}

	// set source to be our id
	transmit_message.set_src(goby::util::as<unsigned>(my_id));

	// set tranmission destination, 0 is broadcast
	if ( transmission_dest == 0 )
		transmit_message.set_dest(goby::acomms::BROADCAST_ID);
	else
		transmit_message.set_dest( transmission_dest );

	if ( transmission_rate == 0 ) {
		// if using fsk, take up to 32 bytes by truncating if necessary
		int data_size = transmission_data.size();
		if ( data_size > 32 ) {
			transmit_message.add_frame( transmission_data.data(), 32 );
		} else {
			transmit_message.add_frame( transmission_data.data(), data_size );
		}
		transmission_data = "";
	} else if ( transmission_rate == 2 ) {
		// if psk, take up to 192 bytes in 64 byte frames
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
	} else if ( transmission_rate == 100 ) {
		// if mini packet, just take up to the first two bytes
		// truncating done in goby
		if ( transmission_data.size() ==1 )
			transmit_message.add_frame( transmission_data.data(), 1 );
		else if ( transmission_data.size() >= 2 ) {
			transmit_message.add_frame( transmission_data.data(), 2 );
		}
	}

	transmit_message.set_ack_requested(false);

	// send transmission to modem
	driver->handle_initiate_transmission( transmit_message );

	// publish transmit information to moosdb
    lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info;
    transmit_info.vehicle_name = my_name;
    transmit_info.rate = transmission_rate;
    transmit_info.dest = transmit_message.dest();
    transmit_info.num_frames = transmit_message.frame_size();
    m_Comms.Notify("ACOMMS_TRANSMIT_SIMPLE", transmit_info.serializeToString());
}

// handle incoming data received or statistics from the modem
void acomms_driver::handle_data_receive(
		const goby::acomms::protobuf::ModemTransmission& data_msg) {
	// take out endlines and publish the received protobuf
	string publish_me = data_msg.DebugString();
	while (publish_me.find("\n") != string::npos) {
		publish_me.replace(publish_me.find("\n"), 1, "<|>");
	}
	m_Comms.Notify("ACOMMS_RECEIVED_ALL", publish_me);

	// look at the number of receive statistics in the message
	int num_stats = data_msg.ExtensionSize(micromodem::protobuf::receive_stat);
	if (num_stats == 1) {
		// psk or mini only
		publishReceivedInfo(data_msg, 0);
	} else if (num_stats == 2) {
		// fsk with mini packet in front - publish the fsk statistics
		publishReceivedInfo(data_msg, 1);
	} else {
		// should not have been
		stringstream ss;
		ss << "Error handling ModemTransmission - contained " << num_stats
				<< "statistics.";
		publishWarning(ss.str());
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
		receive_set_time = MOOSTime();
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

// publish the info we want on received statistics
void acomms_driver::publishReceivedInfo( goby::acomms::protobuf::ModemTransmission trans, int index ) {
	// get statistics out of the transmission
	micromodem::protobuf::ReceiveStatistics stat = trans.GetExtension( micromodem::protobuf::receive_stat, index );

	lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
	if ( stat.psk_error_code() == 2 ) {
		receive_info.num_frames = stat.number_frames();
		receive_info.num_bad_frames = receive_info.num_frames;
		receive_info.num_good_frames = 0;
	} else {
		receive_info.num_frames = stat.number_frames();
		receive_info.num_bad_frames = stat.number_bad_frames();
		receive_info.num_good_frames = receive_info.num_frames - receive_info.num_bad_frames;
	}
	receive_info.vehicle_name = my_name;
	receive_info.source = stat.source();
	if ( trans.type() == goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC ) {
		if ( trans.HasExtension( micromodem::protobuf::type ) ) {
			micromodem::protobuf::TransmissionType type = trans.GetExtension( micromodem::protobuf::type );
			if ( type == micromodem::protobuf::MICROMODEM_MINI_DATA )
				receive_info.rate = 100;
			else {
				publishWarning( "unrecognized transmission type" );
				receive_info.rate = -1;
			}
		} else {
			publishWarning( "missing driver specific transmission type extension" );
			receive_info.rate = -1;
		}
	} else {
		receive_info.rate = stat.rate();
	}
	m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());

	m_Comms.Notify("ACOMMS_SNR_OUT", stat.snr_out());
	m_Comms.Notify("ACOMMS_SNR_IN",stat.snr_in());
	m_Comms.Notify("ACOMMS_DQR",stat.data_quality_factor());

	if ( trans.frame_size() > 0 ) {
		string frame_string = trans.frame(0);
		for ( int i=1; i<trans.frame_size(); i++ ) {
			frame_string += trans.frame(i);
		}
		m_Comms.Notify("ACOMMS_RECEIVED_DATA", frame_string);
	}
}

void acomms_driver::publishWarning( std::string message ) {
	m_Comms.Notify("ACOMMS_DRIVER_WARNING", message );
}

void acomms_driver::publishStatus( std::string status_update ) {
	status = status_update;
	m_Comms.Notify("ACOMMS_DRIVER_STATUS", status_update );
	status_set_time = MOOSTime();
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
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "IRE,1");

	goby::acomms::connect( &driver->signal_receive, boost::bind(&acomms_driver::handle_data_receive, this, _1) );
	goby::acomms::connect( &driver->signal_raw_incoming, boost::bind(&acomms_driver::handle_raw_incoming, this, _1) );

	driver->startup(cfg);
	driver_ready = true;
	driver_initialized = true;

	publishStatus("ready");
}
