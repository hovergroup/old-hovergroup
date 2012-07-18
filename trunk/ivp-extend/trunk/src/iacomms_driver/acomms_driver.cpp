/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <sstream>
#include "acomms_driver.h"
#include "acomms_messages.h"

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

	use_psk_for_minipackets = false;

	transmission_pulse_range = 50;
	transmission_pulse_duration = 5;

	receive_pulse_range = 20;
	receive_pulse_duration = 3;
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
   bool new_transmit = false;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      string key = msg.GetKey();

      if ( key == "ACOMMS_TRANSMIT_RATE" ) {
    	  transmission_rate = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DEST" ) {
    	  transmission_dest = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DATA" ) {
    	  transmission_data = msg.GetString();
    	  new_transmit = true;
//    	  transmit_data( false );
//      } else if ( key == "ACOMMS_TRANSMIT_DATA_BINARY" ) {
//    	  transmission_data = msg.GetString();
//    	  transmit_code = 2;
////    	  transmit_data( true );
      } else if ( key == "LOGGER_DIRECTORY" && !driver_initialized ) {
//          my_name = msg.GetCommunity();
          startDriver( msg.GetString() );
      } else if ( key == "NAV_X" ) {
    	  m_navx = msg.GetDouble();
      } else if ( key == "NAV_Y" ) {
    	  m_navy = msg.GetDouble();
      }
   }

   if ( new_transmit )
	   transmit_data();
	
   return(true);
}

void acomms_driver::RegisterVariables() {
	m_Comms.Register( "ACOMMS_TRANSMIT_RATE", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DEST", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA", 0 );
    m_Comms.Register( "LOGGER_DIRECTORY", 1 );
    m_Comms.Register( "NAV_X", 1 );
    m_Comms.Register( "NAV_Y", 1 );
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
   m_MissionReader.GetConfigurationParam("PSK_minipackets", use_psk_for_minipackets);
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
	if ( status=="transmitting" && MOOSTime()-transmit_set_time > 8 ) {
		publishWarning("Timed out in transmitting state.");
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

void acomms_driver::transmit_data() {
	if ( !driver_ready ) {
		publishWarning("Driver not ready");
		return;
	}

	if ( transmission_data.size() == 0 ) {
		publishWarning("No transmission data");
		return;
	}

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

	vector<unsigned char> transmitted_data;
	if ( transmission_rate == 0 ) {
		// if using fsk, take up to 32 bytes by truncating if necessary
		int data_size = transmission_data.size();
		if ( data_size > 32 )
			data_size = 32;
		transmit_message.add_frame( transmission_data.data(), data_size );

		// save transmitted data
		transmitted_data = vector<unsigned char> (data_size, 0);
		memcpy( &transmitted_data[0], transmission_data.data(), data_size);

		// clear transmission data
		transmission_data = "";

	} else if ( transmission_rate == 2 || transmission_rate == 1) {
		// if psk, take up to 192 bytes in 64 byte frames
		int total_size = transmission_data.size();
		if ( total_size > 192 ) // max size across 3 frames
			total_size = 192;
		transmitted_data = vector<unsigned char> (total_size, 0);

		// while more than 64 bytes left, add another 64 byte frame
		while ( total_size-64 > 0 ) {
			transmit_message.add_frame( transmission_data.data(), 64 );
			int index = (transmit_message.frame_size()-1)*64;
			// save transmitted data
			memcpy( &transmitted_data[index], transmission_data.data(), 64 );
			transmission_data = transmission_data.substr( 64, transmission_data.size()-64 );
			total_size-=64;
		}
		// when 64 bytes or less left, copy remaining data
		transmit_message.add_frame( transmission_data.data(), total_size );
		int index = (transmit_message.frame_size()-1)*64;
		memcpy( &transmitted_data[index], transmission_data.data(), total_size );

		// clear transmission data
		transmission_data = "";

	} else if ( transmission_rate == 100 ) {
		// if mini packet, just take up to the first two bytes
		// truncating done in goby
		if ( transmission_data.size() == 1 ) {
			transmission_data.insert(0,0x00);
			publishWarning("Only passed one byte for minipacket, packing with 0x00.");
		}
		transmit_message.add_frame( transmission_data.data(), 2 );
		transmit_message.mutable_frame(0)->at(0) &= 0x1f;
		transmitted_data = vector<unsigned char> (2,0);
		memcpy( &transmitted_data[0], transmit_message.frame(0).data(), 2 );
	} else {
		stringstream ss;
		ss << "Requested rate " << transmission_rate << " is not supported.";
		publishWarning(ss.str());
		// unhandled rate
		return;
	}

	publishStatus("transmitting");
	transmit_set_time = MOOSTime();
	driver_ready = false;

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

    // publish transmitted data in hexadecimal format
    stringstream ss;
    for ( int i=0; i<transmitted_data.size(); i++ ) {
    	ss << hex << (int) transmitted_data[i] << ":";
    }
	// take off the trailing colon
    string to_publish = ss.str();
    if ( to_publish.size() > 0 )
    	to_publish.erase( --to_publish.end() );
    m_Comms.Notify("ACOMMS_TRANSMITTED_DATA_HEX", to_publish );

    postRangePulse( "transmit", transmission_pulse_range, transmission_pulse_duration );
}

void acomms_driver::postRangePulse( string label, double range, double duration, string color ) {
	XYRangePulse pulse;
	pulse.set_x(m_navx);
	pulse.set_y(m_navy);
	pulse.set_label(label);
	pulse.set_rad(range);
	pulse.set_duration(duration);
	pulse.set_time(MOOSTime());
	pulse.set_color("edge", color);
	pulse.set_color("fill", color);

	m_Comms.Notify("VIEW_RANGE_PULSE", pulse.get_spec());
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
				<< " statistics.";
		publishWarning(ss.str());

		// try to just publish what we can
		publishReceivedInfo( data_msg, -1 );
	}
	driver_ready = true;
	publishStatus("ready");
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
	} else if ( descriptor == "IRE" ) {
		m_Comms.Notify("ACOMMS_IMPULSE_RESPONSE", msg.raw());
	}
//	} else if ( descriptor == "CST" ) {
//		CST_received = true;
//	} else if ( descriptor == "RXD" ) {
//		RXD_received = true;
//	}
////	} else {
////		cout << "unhandled raw msg with descriptor " << descriptor << endl;
////	}
//	if ( !driver_ready && CST_received && RXD_received ) {
//		CST_received = false;
//		RXD_received = false;
//		driver_ready = true;
//		publishStatus("ready");
//	}
}

// publish the info we want on received statistics
void acomms_driver::publishReceivedInfo( goby::acomms::protobuf::ModemTransmission trans, int index ) {
	if ( index == -1 ) {
		lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
		receive_info.num_frames = trans.frame_size();
		receive_info.num_bad_frames = receive_info.num_frames;
		receive_info.num_good_frames = 0;
		receive_info.rate = trans.rate();
		receive_info.source = trans.src();
		receive_info.vehicle_name = my_name;

		// notify data received in ascii format
		string frame_string = trans.frame(0);
		for ( int i=1; i<trans.frame_size(); i++ ) {
			frame_string += trans.frame(i);
			if ( trans.frame(i).empty() ) {
				receive_info.num_bad_frames--;
				receive_info.num_good_frames++;
			}
		}
		m_Comms.Notify("ACOMMS_RECEIVED_DATA", frame_string);

		// notify data received in hex format
		vector<unsigned char> received_data (frame_string.size(), 0);
		memcpy( &received_data[0], frame_string.data(), frame_string.size() );
		stringstream ss;
		for ( int i=0; i<received_data.size(); i++ ) {
			ss << hex << (int) received_data[i] << ":";
		}
		// take off the trailing colon
	    string to_publish = ss.str();
	    if ( to_publish.size() > 0 )
	    	to_publish.erase( --to_publish.end() );
		m_Comms.Notify("ACOMMS_RECEIVED_DATA_HEX", to_publish);

		m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());

	} else {
		// get statistics out of the transmission
		micromodem::protobuf::ReceiveStatistics stat = trans.GetExtension( micromodem::protobuf::receive_stat, index );

		// file simplified receive info
		lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
		if ( stat.psk_error_code() == micromodem::protobuf::BAD_CRC_DATA_HEADER ||
				stat.psk_error_code() == micromodem::protobuf::BAD_MODULATION_HEADER ||
				stat.psk_error_code() == micromodem::protobuf::MISSED_START_OF_PSK_PACKET ) {
			receive_info.num_frames = stat.number_frames();
			receive_info.num_bad_frames = receive_info.num_frames;
			receive_info.num_good_frames = 0;
		} else {
			receive_info.num_frames = stat.number_frames();
			receive_info.num_bad_frames = stat.number_bad_frames();
			receive_info.num_good_frames = receive_info.num_frames - receive_info.num_bad_frames;
		}
		// for debugging, not sure if this is ever a problem
		if( receive_info.num_good_frames > 0 && trans.frame_size() != stat.number_frames() ) {
			stringstream ss;
			ss << "calculated " << receive_info.num_good_frames << " good frames and "
					<< receive_info.num_frames << " total, but found " <<
					stat.number_frames();
			publishWarning( ss.str() );
		}

		receive_info.vehicle_name = my_name;
		receive_info.source = stat.source();
		// check for mini data type to set custom rate, otherwise use reported rate
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

		// notify individual statistics that are considered useful to know about or convienient to
		// see outside ACOMMS_RECEIVED_ALL
		m_Comms.Notify("ACOMMS_SNR_OUT", stat.snr_out());
		m_Comms.Notify("ACOMMS_SNR_IN",stat.snr_in());
		m_Comms.Notify("ACOMMS_DQR",stat.data_quality_factor());

		if ( trans.frame_size() > 0 ) {
			// notify data received in ascii format
			string frame_string = trans.frame(0);
			for ( int i=1; i<trans.frame_size(); i++ ) {
				frame_string += trans.frame(i);
			}
			m_Comms.Notify("ACOMMS_RECEIVED_DATA", frame_string);

			// notify data received in hex format
			vector<unsigned char> received_data (frame_string.size(), 0);
			memcpy( &received_data[0], frame_string.data(), frame_string.size() );
			stringstream ss;
			for ( int i=0; i<received_data.size(); i++ ) {
				ss << hex << (int) received_data[i] << ":";
			}
			// take off the trailing colon
		    string to_publish = ss.str();
		    if ( to_publish.size() > 0 )
		    	to_publish.erase( --to_publish.end() );
			m_Comms.Notify("ACOMMS_RECEIVED_DATA_HEX", to_publish );
		}

		// create a range pulse
		if ( receive_info.num_frames > 0 && receive_info.num_bad_frames == 0 ) {
			// for believed good receipts, green pulse
			postRangePulse( "receipt good", receive_pulse_range, receive_pulse_duration, "green" );
		} else {
			// red pulse for receipts we think are bad
			postRangePulse( "receipt bad", receive_pulse_range, receive_pulse_duration, "red" );
		}
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

bool acomms_driver::file_exists( std::string filename ) {
	ifstream my_file(filename.c_str());
	if ( my_file.good() ) {
		my_file.close();
		return true;
	} else {
		my_file.close();
		return false;
	}
}

void acomms_driver::startDriver( std::string logDirectory ) {
	cout << "opening goby log file..." << endl;
	int file_index = 0;
	string filename = logDirectory + "/goby_log_" +
		boost::lexical_cast<string>( file_index ) + ".txt";
	while ( file_exists( filename ) ) {
		cout << filename << " already exists." << endl;
		file_index++;
		filename = logDirectory + "/goby_log_" +
			boost::lexical_cast<string>( file_index ) + ".txt";
	}
	verbose_log.open(filename.c_str());
	cout << "Goby logging to " << filename << endl;

	cfg.set_serial_port( port_name );
	cfg.set_modem_id( my_id );

	goby::glog.set_name( "iacomms_driver" );
	goby::glog.add_stream( goby::common::logger::DEBUG1, &std::clog );
	goby::glog.add_stream( goby::common::logger::DEBUG3, &verbose_log );

	std::cout << "Starting WHOI Micro-Modem MMDriver" << std::endl;
	driver = new goby::acomms::MMDriver;
//	driver = new MMDriver_extended;
	// turn data quality factor message on
	// (example of setting NVRAM configuration)
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DQF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MFD,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "SHF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DOP,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "IRE,1");
	if ( use_psk_for_minipackets )
		cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MOD,1");
	else
		cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MOD,0");

	goby::acomms::connect( &driver->signal_receive, boost::bind(&acomms_driver::handle_data_receive, this, _1) );
	goby::acomms::connect( &driver->signal_raw_incoming, boost::bind(&acomms_driver::handle_raw_incoming, this, _1) );

	driver->startup(cfg);
	driver_ready = true;
	driver_initialized = true;

	publishStatus("ready");
}
