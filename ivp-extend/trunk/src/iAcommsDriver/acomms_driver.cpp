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

	receive_set_time = 0;
	status_set_time = 0;

	use_psk_for_minipackets = false;
	enable_one_way_ranging = false;
	enable_range_pulses = true;
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
    	  if (!m_transmission.setRate(msg.GetDouble())) {
    		  publishWarning("Transmit rate not supported.");
    	  }
      } else if ( key == "ACOMMS_TRANSMIT_DEST" ) {
    	  m_transmission.setDest(msg.GetDouble());
      } else if ( key == "ACOMMS_TRANSMIT_DATA" &&
    		  msg.GetSource() != GetAppName() ) {
    	  m_transmission.fillData(msg.GetString());
    	  new_transmit = true;
      } else if ( key == "ACOMMS_TRANSMIT_DATA_BINARY" &&
    		  msg.GetSource() != GetAppName() ) {
    	  m_transmission.fillData(msg.GetString());
    	  new_transmit = true;
      } else if ( key == "LOGGER_DIRECTORY" && !driver_initialized && connect_complete ) {
    	  // get log directory from plogger that we will also use
          startDriver( msg.GetString() );
      } else if ( key == "NAV_X" ) {
    	  m_navx = msg.GetDouble();
      } else if ( key == "NAV_Y" ) {
    	  m_navy = msg.GetDouble();
      } else if ( key == "ACOMMS_TRANSMIT" ){
    	  m_transmission.parseFromString(msg.GetString());
    	  new_transmit=true;
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
    m_Comms.Register( "ACOMMS_TRANSMIT", 0 );
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_driver::OnConnectToServer()
{
   m_MissionReader.GetConfigurationParam("PortName", port_name);
   m_MissionReader.GetConfigurationParam("ID", my_id);
   m_MissionReader.GetValue("Community", my_name);
   m_MissionReader.GetConfigurationParam("PSK_minipackets", use_psk_for_minipackets);
   m_MissionReader.GetConfigurationParam("enable_ranging", enable_one_way_ranging);
   m_MissionReader.GetConfigurationParam("show_range_pulses", enable_range_pulses);

   // post these to make sure they are the correct type
   unsigned char c = 0x00;
   m_Comms.Notify("ACOMMS_TRANSMIT_DATA", ""); // string
//   m_Comms.Notify("ACOMMS_RECEIVED_DATA", &c, 1); // binary string
   m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", &c, 1); // binary string
   m_Comms.Notify("ACOMMS_TRANSMIT", &c, 1); // binary string

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
	double transmit_timeout;
	switch ( m_transmission.getRate() ) {
	case HoverAcomms::MINI:
		transmit_timeout = 2;
		break;
	default:
		transmit_timeout = 8;
	}
	if ( status=="transmitting" && MOOSTime()-transmit_set_time > transmit_timeout ) {
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

// new data transmission
void acomms_driver::transmit_data() {
	// check driver status
	if ( !driver_ready ) {
		publishWarning("Driver not ready");
		return;
	}

	// check that we have transmission data
	if ( m_transmission.getData().size() == 0 ) {
		publishWarning("No transmission data");
		return;
	}

	// set status to transmitting
	publishStatus("transmitting");
	transmit_set_time = MOOSTime();
	driver_ready = false;

	goby::acomms::protobuf::ModemTransmission trans = m_transmission.getProtobuf();

	trans.set_ack_requested(false);

	driver->handle_initiate_transmission( trans );

    m_Comms.Notify("ACOMMS_TRANSMITTED_DATA_HEX", m_transmission.getHexData());
    m_Comms.Notify("ACOMMS_TRANSMISSION", m_transmission.getLoggableString());

    m_transmission.m_vehicleName = my_name;
    std::string to_publish = m_transmission.serializeWithSource();
    m_Comms.Notify("ACOMMS_TRANSMITTED", to_publish.data(), to_publish.size());

    // post transmission range pulse
    postRangePulse( "transmit", transmission_pulse_range,
    		transmission_pulse_duration, "cyan");
}

// post range pulse for pMarineViewer
void acomms_driver::postRangePulse( string label, double range, double duration, string color ) {
	if ( !enable_range_pulses ) return;

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
	HoverAcomms::AcommsReception reception;
	reception.copyFromProtobuf(data_msg);
	reception.m_vehicleName = my_name;

	bool ok = true;
	std::string debug_msg = reception.verify(ok);
	if (!ok) {
		publishWarning(debug_msg);
	} else {
		std::string serialized = reception.serializeWithSource();
		m_Comms.Notify("ACOMMS_RECEIVED", serialized.data(), serialized.size());
		m_Comms.Notify("ACOMMS_RECEIVED_ALL", reception.getLoggableString());

		m_Comms.Notify("ACOMMS_SOURCE_ID", (double) reception.getSource());
		m_Comms.Notify("ACOMMS_DEST_ID", (double) reception.getDest());
		m_Comms.Notify("ACOMMS_RATE", (double) reception.getRate());
		m_Comms.Notify("ACOMMS_ONE_WAY_TRAVEL_TIME", reception.getRangingTime());
		std::string data = reception.getData();
		m_Comms.Notify("ACOMMS_RECEIVED_DATA", data.data(), data.size());
		m_Comms.Notify("ACOMMS_RECEIVED_DATA_HEX", reception.getHexData());
		m_Comms.Notify("ACOMMS_BAD_FRAMES", reception.getBadFrameListing());

		micromodem::protobuf::ReceiveStatistics stat = reception.getStatistics(1);
		m_Comms.Notify("ACOMMS_SNR_OUT", (double) stat.snr_out());
		m_Comms.Notify("ACOMMS_SNR_IN", (double) stat.snr_in());
		m_Comms.Notify("ACOMMS_DQF", (double) stat.data_quality_factor());
		m_Comms.Notify("ACOMMS_STDDEV_NOISE", (double) stat.stddev_noise());
		m_Comms.Notify("ACOMMS_MSE", (double) stat.mse_equalizer());

		HoverAcomms::ReceiptStatus status = reception.getStatus();
		m_Comms.Notify("ACOMMS_RECEIVED_STATUS", (double) status);
		if (status==HoverAcomms::GOOD) {
			postRangePulse(my_name+"_receipt_good", receive_pulse_range,
					receive_pulse_duration, "green");
		} else if (status==HoverAcomms::PARTIAL) {
			postRangePulse(my_name+"_receipt_partial", receive_pulse_range,
					receive_pulse_duration, "yellow");
		} else {
			postRangePulse(my_name+"_receipt_bad", receive_pulse_range,
					receive_pulse_duration, "red");
		}
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
	goby::glog.set_name( "iAcommsDriver" );
	goby::glog.add_stream( goby::common::logger::DEBUG1, &std::clog );
	goby::glog.add_stream( goby::common::logger::DEBUG3, &verbose_log );

	// set serial port
	cfg.set_serial_port( port_name );
	cfg.set_modem_id( my_id );

	std::cout << "Starting WHOI Micro-Modem MMDriver" << std::endl;
	driver = new goby::acomms::MMDriver;

	// set configuration variables

	// various statistics
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DQF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MFD,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "SHF,1");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DOP,1");

	// impulse response
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "IRE,1");

	// gain control
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "AGC,0");
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "AGN,250");

	// ranging
	if ( enable_one_way_ranging )
		cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "SNV,1");
	else
		cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "SNV,0");

	// number of CTOs before hard reboot
	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "NRV,0");

	// psk vs. fsk minipackets
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
