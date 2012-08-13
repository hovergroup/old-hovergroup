/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <sstream>
#include <algorithm>
#include "acomms_driver.h"
#include "acomms_messages.h"

using namespace std;

//---------------------------------------------------------
// Constructor

acomms_driver::acomms_driver()
{
	driver_ready = false;
    driver_initialized = false;
    connect_complete = false;
    port_name = "/dev/ttyUSB0";
    my_name = "default_name";

	transmission_rate = 0;
	transmission_dest = 0;

	receive_set_time = 0;
	status_set_time = 0;

	use_psk_for_minipackets = false;
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
    	  // set transmission rate
    	  transmission_rate = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DEST" ) {
    	  // set transmission destination
    	  transmission_dest = (int) msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT_DATA" &&
    		  msg.GetSource() != GetAppName() ) {
    	  // transmission data, excluding our own
    	  transmission_data = msg.GetString();
    	  new_transmit = true;
      } else if ( key == "ACOMMS_TRANSMIT_DATA_BINARY" &&
    		  msg.GetSource() != GetAppName() ) {
    	  // transmission data, excluding our own
    	  transmission_data = msg.GetString();
    	  new_transmit = true;
      } else if ( key == "LOGGER_DIRECTORY" && !driver_initialized && connect_complete ) {
    	  // get log directory from plogger that we will also use
          startDriver( msg.GetString() );
      } else if ( key == "NAV_X" ) {
    	  m_navx = msg.GetDouble();
      } else if ( key == "NAV_Y" ) {
    	  m_navy = msg.GetDouble();
      }
   }

   // send transmission after reading all variables
   if ( new_transmit )
	   transmit_data();
	
   return(true);
}

void acomms_driver::RegisterVariables() {
	m_Comms.Register( "ACOMMS_TRANSMIT_RATE", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DEST", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA_BINARY", 0 );
    m_Comms.Register( "LOGGER_DIRECTORY", 1 );
    m_Comms.Register( "NAV_X", 1 );
    m_Comms.Register( "NAV_Y", 1 );
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_driver::OnConnectToServer()
{
   m_MissionReader.GetConfigurationParam("PortName", port_name);
   m_MissionReader.GetConfigurationParam("ID", my_id);
   m_MissionReader.GetValue("Community", my_name);
   m_MissionReader.GetConfigurationParam("PSK_minipackets", use_psk_for_minipackets);

   // post these to make sure they are the correct type
   unsigned char c = 0x00;
   m_Comms.Notify("ACOMMS_TRANSMIT_DATA", ""); // string
   m_Comms.Notify("ACOMMS_RECEIVED_DATA", &c, 1); // binary string
   m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", &c, 1); // binary string

   RegisterVariables();

   // driver not started yet
   publishStatus("not running");

   connect_complete = true;

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool acomms_driver::Iterate()
{
	// run the driver
	driver->do_work();

	// receive status timeout
	if ( status=="receiving" && MOOSTime()-receive_set_time > 8 ) {
		publishWarning("Timed out in receiving state.");
		driver_ready = true;
		publishStatus("ready");
	}

	// transmit status timeout
	if ( status=="transmitting" && MOOSTime()-transmit_set_time > 8 ) {
		publishWarning("Timed out in transmitting state.");
		driver_ready = true;
		publishStatus("ready");
	}

	// status gets updated every 5 seconds
	if ( MOOSTime()-status_set_time > 5 ) {
		publishStatus( status );
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool acomms_driver::OnStartUp()
{
	return(true);
}

// pack data into frames
vector<unsigned char> acomms_driver::packMessage( int max_frames, int frame_size,
		goby::acomms::protobuf::ModemTransmission * msg ) {
	vector<unsigned char> packed_data;

	// something's wrong
	if ( transmission_data.size()==0 || max_frames<=0 || frame_size<=0 ) {
		publishWarning("Fatal error in call to packMessage.");
		exit(1);
		return packed_data;
	}

	// just a single frame
	if ( transmission_data.size() <= frame_size ) {
		msg->add_frame( transmission_data.data(), transmission_data.size() );
		packed_data = vector<unsigned char> (transmission_data.size(), 0);
		memcpy( &packed_data[0], transmission_data.data(), transmission_data.size() );
	} else { // multiple frames
		int filled_size = 0;
		// pack in full frames
		while ( transmission_data.size() > frame_size && msg->frame_size()<max_frames ) {
			msg->add_frame( transmission_data.data(), frame_size );
			transmission_data = transmission_data.substr( frame_size,
					transmission_data.size()-frame_size );
			filled_size+=frame_size;
		}
		// fill up last frame or use remaining data
		if ( transmission_data.size()>0 && msg->frame_size()<max_frames ) {
			int leftover = min( frame_size, (int) transmission_data.size() );
			msg->add_frame( transmission_data.data(), leftover );
			filled_size+=leftover;
		}
		// copy out what made it in
		packed_data = vector<unsigned char> (filled_size, 0);
		for ( int i=0; i<msg->frame_size(); i++ ) {
			memcpy( &packed_data[i*frame_size], msg->frame(i).data(), msg->frame(i).size() );
		}
	}
	transmission_data == "";
	return packed_data;
}

// new data transmission
void acomms_driver::transmit_data() {
	// check driver status
	if ( !driver_ready ) {
		publishWarning("Driver not ready");
		return;
	}

	// check that we have transmission data
	if ( transmission_data.size() == 0 ) {
		publishWarning("No transmission data");
		return;
	}

	// construct new goby modem transmission
	goby::acomms::protobuf::ModemTransmission transmit_message;

	// set transmission type
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

	// pack in data based on transmission type
	// transmitted_data = packMessage(num_frames, frame_size, ModemTransmission)
	vector<unsigned char> transmitted_data;
	switch ( transmission_rate ) {
	case 0:
		transmitted_data = packMessage(1, 32, &transmit_message);
		break;

	case 1:
		transmitted_data = packMessage(3, 64, &transmit_message);
		break;

	case 2:
		transmitted_data = packMessage(3, 64, &transmit_message);
		break;

	case 3:
		transmitted_data = packMessage(2, 256, &transmit_message);
		break;

	case 4:
		transmitted_data = packMessage(2, 256, &transmit_message);
		break;

	case 5:
		transmitted_data = packMessage(8, 256, &transmit_message);
		break;

	case 6:
		transmitted_data = packMessage(6, 32, &transmit_message);
		break;

	default:
		stringstream ss;
		ss << "Requested rate " << transmission_rate << " is not supported.";
		publishWarning(ss.str());
		// unhandled rate
		return;
	}

	// set status to transmitting
	publishStatus("transmitting");
	transmit_set_time = MOOSTime();
	driver_ready = false;

	// ack not requested
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
    	ss << hex << (int) transmitted_data[i];
		if ( i < transmitted_data.size()-1 )
			ss << ":";
    }
    m_Comms.Notify("ACOMMS_TRANSMITTED_DATA_HEX", ss.str() );

    // post transmission range pulse
    postRangePulse( "transmit", transmission_pulse_range, transmission_pulse_duration );
}

// post range pulse for pMarineViewer
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
		// end of transmission, change status back to ready
		driver_ready = true;
		publishStatus("ready");
	} else if ( descriptor == "RXP" ) {
		// receive start, set status and flags
		driver_ready = false;
		CST_received = false;
		RXD_received = false;
		publishStatus("receiving");
		receive_set_time = MOOSTime();
	} else if ( descriptor == "IRE" ) {
		// impulse response, post to moosdb
		m_Comms.Notify("ACOMMS_IMPULSE_RESPONSE", msg.raw());
	}
}

void acomms_driver::publishReceivedData( goby::acomms::protobuf::ModemTransmission & trans ) {
	if ( trans.frame_size() > 0 ) {
		// notify data received in ascii format
		string frame_string = trans.frame(0);
		for ( int i=1; i<trans.frame_size(); i++ ) {
			frame_string += trans.frame(i);
		}
		m_Comms.Notify("ACOMMS_RECEIVED_DATA", (void*)frame_string.data(), frame_string.size() );

		// notify data received in hex format
		vector<unsigned char> received_data (frame_string.size(), 0);
		memcpy( &received_data[0], frame_string.data(), frame_string.size() );
		stringstream ss;
		for ( int i=0; i<received_data.size(); i++ ) {
			ss << hex << (int) received_data[i];
			if ( i < received_data.size()-1 )
				ss << ":";
		}
		m_Comms.Notify("ACOMMS_RECEIVED_DATA_HEX", ss.str() );

		if ( trans.HasExtension(micromodem::protobuf::frame_with_bad_crc) ) {
			int num_bad = trans.ExtensionSize(micromodem::protobuf::frame_with_bad_crc);
			if ( num_bad == 0 ) {
				m_Comms.Notify("ACOMMS_BAD_FRAMES", "-1");
				publishWarning("had frame_with_bad_crc extension, size 0");
			} else {
				stringstream ss;
				for ( int i=0; i<num_bad; i++ ) {
					ss << trans.GetExtension(micromodem::protobuf::frame_with_bad_crc, i);
					if ( i<num_bad-1 )
						ss << ",";
				}
				m_Comms.Notify("ACOMMS_BAD_FRAMES", ss.str() );
			}
		} else {
			m_Comms.Notify("ACOMMS_BAD_FRAMES", "-1");
			publishWarning("did not have frame_with_bad_crc extension");
		}
	}
}

// publish the info we want on received statistics
void acomms_driver::publishReceivedInfo( goby::acomms::protobuf::ModemTransmission trans, int index ) {
	if ( index == -1 ) {
		// wrong number of receive statistics

		// construct simple receive info
		lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
		receive_info.num_frames = trans.frame_size();
		receive_info.num_bad_frames = receive_info.num_frames;
		receive_info.num_good_frames = 0;
		receive_info.rate = trans.rate();
		receive_info.source = trans.src();
		receive_info.vehicle_name = my_name;

		string frame_string = trans.frame(0);
		for ( int i=1; i<trans.frame_size(); i++ ) {
			frame_string += trans.frame(i);
			if ( trans.frame(i).empty() ) {
				receive_info.num_bad_frames--;
				receive_info.num_good_frames++;
			}
		}

		m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());

		publishReceivedData( trans );
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
//			stat.set_number_frames(stat.number_frames()-1);
			receive_info.num_frames--;
			receive_info.num_good_frames--;
			publishWarning( "Statistics found an extra frame - decrementing." );
		}

		receive_info.vehicle_name = my_name;
		receive_info.source = stat.source();
		// check for mini data type to set custom rate, otherwise use reported rate
		if ( trans.type() == goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC ) {
			if ( trans.HasExtension( micromodem::protobuf::type ) ) {
				micromodem::protobuf::TransmissionType type = trans.GetExtension( micromodem::protobuf::type );
				if ( type == micromodem::protobuf::MICROMODEM_MINI_DATA ) {
					// mini packet - set rate manually
					receive_info.rate = 100;
				} else {
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
		m_Comms.Notify("ACOMMS_DQF",stat.data_quality_factor());
		m_Comms.Notify("ACOMMS_STDDEV_NOISE", stat.stddev_noise());
		m_Comms.Notify("ACOMMS_MSE", stat.mse_equalizer());

		publishReceivedData( trans );

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


// publish warning
void acomms_driver::publishWarning( std::string message ) {
	m_Comms.Notify("ACOMMS_DRIVER_WARNING", message );
}

// publish status and update locally
void acomms_driver::publishStatus( std::string status_update ) {
	status = status_update;
	m_Comms.Notify("ACOMMS_DRIVER_STATUS", status_update );
	status_set_time = MOOSTime();
}

// check if a file exists
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
	// open goby log file
	cout << "opening goby log file..." << endl;
	int file_index = 0;
	string filename = logDirectory + "/goby_log_" +
		boost::lexical_cast<string>( file_index ) + ".txt";
	// increase filename index if file exists
	while ( file_exists( filename ) ) {
		cout << filename << " already exists." << endl;
		file_index++;
		filename = logDirectory + "/goby_log_" +
			boost::lexical_cast<string>( file_index ) + ".txt";
	}
	verbose_log.open(filename.c_str());
	cout << "Goby logging to " << filename << endl;

	// start goby log
	goby::glog.set_name( "iacomms_driver" );
	goby::glog.add_stream( goby::common::logger::DEBUG1, &std::clog );
	goby::glog.add_stream( goby::common::logger::DEBUG3, &verbose_log );

	// set serial port
	cfg.set_serial_port( port_name );
	cfg.set_modem_id( my_id );

	std::cout << "Starting WHOI Micro-Modem MMDriver" << std::endl;
	driver = new goby::acomms::MMDriver;

	// set configuration variables
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DQF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MFD,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "SHF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DOP,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "IRE,1");
	if ( use_psk_for_minipackets )
		cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MOD,1");
	else
		cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MOD,0");

	// connect receive and raw signals to our functions
	goby::acomms::connect( &driver->signal_receive, boost::bind(&acomms_driver::handle_data_receive, this, _1) );
	goby::acomms::connect( &driver->signal_raw_incoming, boost::bind(&acomms_driver::handle_raw_incoming, this, _1) );

	driver->startup(cfg);
	driver_ready = true;
	driver_initialized = true;

	publishStatus("ready");
}
