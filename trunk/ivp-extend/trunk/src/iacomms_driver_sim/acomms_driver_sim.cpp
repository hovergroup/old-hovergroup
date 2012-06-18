/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver_sim.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "acomms_driver_sim.h"
#include <sstream>
#include <acomms_messages.h>

using namespace std;

//---------------------------------------------------------
// Constructor

acomms_driver_sim::acomms_driver_sim()
{
	driver_ready = false;
    driver_initialized = false;

	transmission_rate = 0;
	transmission_dest = 0;

	receive_set_time = 0;
	status_set_time = 0;

	//Sims
	encoding_time = 1;
	sending_time = 3;
}

//---------------------------------------------------------
// Destructor

acomms_driver_sim::~acomms_driver_sim()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool acomms_driver_sim::OnNewMail(MOOSMSG_LIST &NewMail)
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
        	  transmitting = true;
        	  transmit_data( false );
          } else if ( key == "ACOMMS_TRANSMIT_DATA_BINARY" ) {
        	  transmission_data = msg.GetString();
        	  transmitting = true;
        	  transmit_data( true );
          } else if ( key == "LOGGER_DIRECTORY" && !driver_initialized ) {
              startDriver( msg.GetString() );
          }

          //Sims
          else if (key=="ACOMMS_SIM_SENT_DATA"){
        	  sent_time = MOOSTime();
        	  sent_data = msg.GetString();
        	  receiving = true;
          }else if (key=="NAV_X"){
        	  x = msg.GetDouble();
          }else if (key=="NAV_Y"){
        	  y = msg.GetDouble();
          }
   }
	
   return(true);
}


void acomms_driver_sim::RegisterVariables() {
	m_Comms.Register( "ACOMMS_TRANSMIT_RATE", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DEST", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA", 0 );
	m_Comms.Register( "ACOMMS_TRANSMIT_DATA_BINARY", 0 );
    m_Comms.Register( "LOGGER_DIRECTORY", 0 );

    //Sim
    m_Comms.Register("NAV_X",0);
    m_Comms.Register("NAV_Y",0);
    m_Comms.Register("ACOMMS_SIM_SENT_DATA",0);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_driver_sim::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	 // register for variables here
	   // possibly look at the mission file?
	   m_MissionReader.GetConfigurationParam("ID", my_id);
	   m_MissionReader.GetValue("Community",my_name);
	   // m_Comms.Register("VARNAME", is_float(int));

	   RegisterVariables();

	   publishStatus("not running");

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool acomms_driver_sim::Iterate()
{
   // happens AppTick times per second
	//do_work();
		if ( status=="receiving" && MOOSTime()-receive_set_time > 8 ) {
			publishWarning("Timed out in receiving state.");
			driver_ready = true;
			publishStatus("ready");
		}

		if ( MOOSTime()-status_set_time > 5 ) {
			publishStatus( status );
		}

		if(receiving){
			while(MOOSTime()-sent_time < sending_time){
				//wait
			}
			handle_data_receive(sent_data);
			receiving = false;
		}

	   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool acomms_driver_sim::OnStartUp()
{
   // happens before connection is open

   return(true);
}

//--------------------------------------------------------

void acomms_driver_sim::transmit_data( bool isBinary ) {
	if ( !driver_ready ) {
		publishWarning("Driver not ready");
		return;
	}

	publishStatus("transmitting");
	driver_ready = false;
	start_time = MOOSTime();

	stringstream ss;
	int num_frames = 0;

	//x,y,name,rate,src id,dest id,data, num-frames
	ss <<x<<","<<y<<","
	   <<my_name<<","
	  <<transmission_rate<<","
	  <<my_id<<","
	  <<transmission_dest<<",";

	if ( transmission_rate == 0 ) {
		int data_size = transmission_data.size();
		num_frames = 1;
		if ( data_size > 32 ) {
			ss.write(transmission_data.data(),32);
			ss<<",1"; //num_frames
		} else {
			ss<<transmission_data.data();
			ss<<",1"; //num_frames
		}
		transmission_data = "";
	}

	else if ( transmission_rate == 2 ) {
		for ( int i=0; i<3; i++ ) {
			int data_size = transmission_data.size();
			if ( data_size > 64 ) {
				ss.write(transmission_data.data(),64);
				transmission_data = transmission_data.substr( 64, data_size-64 );
				ss<<":";
				num_frames++;
			} else if ( data_size > 0 ) {
				num_frames++;
				ss<<transmission_data.data();
				ss<<","<<num_frames;
				break;
			}
		}
		transmission_data = "";
	}


	while(MOOSTime()-start_time<encoding_time){
		//wait
		}

	//x,y,name,rate,src id,dest id,data, num-frames
	m_Comms.Notify("ACOMMS_SIM_SENT_DATA",ss.str());
	ss.str("");

	//x=100,y=-50,radius=40,duration=15,fill=0.25,
	//fill color=green,label=04,time=@MOOSTIME
	ss<<"x="<<x<<",y="<<y<<",time="<<MOOSTime();
	m_Comms.Notify("VIEW_RANGE_PULSE", ss.str());

    lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info;
    transmit_info.vehicle_name = my_name;
//    transmit_info.rate = transmit_message.rate();
    transmit_info.rate = transmission_rate;
    transmit_info.dest = transmission_dest;
    transmit_info.num_frames = num_frames;

    m_Comms.Notify("ACOMMS_TRANSMIT_SIMPLE", transmit_info.serializeToString());
}

void acomms_driver_sim::handle_data_receive(string sent_data){
	goby::acomms::protobuf::ModemTransmission* data_msg;
	micromodem::protobuf::ReceiveStatistics* cst;
	micromodem::protobuf::ReceiveStatistics* cst_mini;

//	stringstream ss;
//	ss.str(sent_data.c_str());
//	double srcx,srcy,rate,srcid,destid,num_frames;
//	char temp;
//	string data,sender;
//	//x,y,name,rate,src id,dest id,data,num-frames
//	ss >> srcx; ss >> temp;
//	ss >> srcy; ss >> temp;
//	ss >> sender; sender = sender.substr(0,sender.size()-2);
//	ss>>rate; ss>>temp;
//	ss>>srcid; ss>>temp;
//	ss>>destid; ss>>temp;
//	ss>>data; data = data.substr(0,data.size()-2);
//	ss>>num_frames;

	cout<<"Sent from: "<<sender
		<<" at "<< srcx<<","<<srcy<<endl;

	if(rate==0){
		cst_mini = data_msg->AddExtension(micromodem::protobuf::receive_stat);
		cst = data_msg->AddExtension(micromodem::protobuf::receive_stat);
	}
	else if(rate==2){
		cst = data_msg->AddExtension(micromodem::protobuf::receive_stat);
	}

	std::vector probs = getProbabilities(x,y,srcx,srcy,rate);
	if(rollDice(probs[0])){ //sync
		cst->set_rate(rate);
		cst->set_source(srcid);
		cst->set_dest(destid);
		cst->set_number_frames(num_frames);

		if(rollDice(probs[1])){ //header
			if(rollDice(probs[2])){ //modulation
							for(int i=0;i<num_frames;i++){
								if(rollDice(probs[3])){//frame

								}
							}
						}
			else{

			}
		}
		else{


		}

		string publish_me = data_msg->DebugString();
		while (publish_me.find("\n") != string::npos) {
				publish_me.replace(publish_me.find("\n"), 1, "<|>");
			}
		m_Comms.Notify("ACOMMS_RECEIVED_ALL", publish_me);

		lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
		if ( cst->psk_error_code() == 2 ) {
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
		receive_info.rate = stat.rate();
		m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());

		m_Comms.Notify("ACOMMS_SNR_OUT", stat.snr_out);
		m_Comms.Notify("ACOMMS_SNR_IN",stat.snr_in);
		m_Comms.Notify("ACOMMS_DQR",stat.data_quality_factor);

		m_Comms.Notify("ACOMMS_RECEIVED_DATA", data);

	}else{
		//sync loss
	}
}

bool acomms_driver_sim::rollDice(double p){
    srand((unsigned) time(NULL));
    int face = rand() % 1;
    if(face<p){
    	return true;
    }
    else{
    	return false;
    }

}

std::vector<double> acomms_driver_sim::getProbabilities(double myx, double my, double x, double y, double rate)
{
	std::vector<double> probabilities;
	double sync_loss = 0.2;
	double header_loss = 0.01;
	double modulation_loss = 0.05;
	double frame_loss = 0.05;
	probabilities.push_back(sync_loss);
	probabilities.push_back(header_loss);
	probabilities.push_back(modulation_loss);
	probabilities.push_back(frame_loss);
	return probabilities;
}

//string acomms_driver_sim::SerializeToString(ModemStat my_stat){
//	stringstream out;
//	out<<"time: 1338220425000000<|>time_source: MODEM_TIME"
//	  <<"<|> [micromodem.protobuf.receive_stat] {"
//	  <<"<|> mode: "<<my_stat.mode<<"<|>  time: 155345.0000"
//	  <<"<|> clock_mode: NO_SYNC_TO_PPS_AND_CCCLK_BAD"
//	  <<"<|> mfd_peak: "<<my_stat.mfd_peak<<"<|>  mfd_power: "<<my_stat.mfd_power
//	  <<"<|> mfd_ratio: "<<my_stat.mfd_ratio<<"<|>  spl: "
//	  <<my_stat.spl<<"<|>  shf_agn: "<<my_stat.shf_agn<<"<|>  shf_ainpshift: "
//	  <<my_stat.shf_ainpshift<<"<|> shf_ainshift: "<<my_stat.shf_ainshift
//	  <<"<|>  shf_mfdshift: "<<my_stat.shf_mfdshift<<"<|>  shf_p2bshift: "
//	  <<my_stat.shf_p2bshift<<"<|>  rate: "<<my_stat.rate
//	  <<"<|> source: "<<my_stat.source<<"<|>  dest: "
//	  <<my_stat.dest<<"<|>  psk_error_code: "
//	  <<my_stat.psk_error_code<<"<|>  packet_type: "<<my_stat.packet_type
//	  <<"<|> number_frames: "<<my_stat.number_frames
//	  <<"<|>  number_bad_frames: "<<my_stat.number_bad_frames
//	  <<"<|>  snr_rss: "<<my_stat.snr_rss<<"<|>  snr_in: "<<my_stat.snr_in
//	  <<"<|> snr_out: "<<my_stat.snr_out<<"<|>  snr_symbols: "
//	  <<my_stat.snr_symbols<<"<|>  mse_equalizer: "
//	  <<my_stat.mse_equalizer<<"<|>  data_quality_factor: "
//	  <<my_stat.data_quality_factor<<"<|> doppler: "
//	  <<my_stat.doppler<<"<|>  stddev_noise: "
//	  <<my_stat.stddev_noise<<"<|>  carrier_freq: 25120<|>  bandwidth: 4000<|>version: 0<|>}<|>"
//
//	return out.str();
//}

//void acomms_driver_sim::handle_data_receive( const goby::acomms::protobuf::ModemTransmission& data_msg ) {
//	string publish_me = data_msg.DebugString();
//    while ( publish_me.find("\n") != string::npos ) {
//            publish_me.replace( publish_me.find("\n"), 1, "<|>" );
//    }
//    m_Comms.Notify("ACOMMS_RECEIVED_ALL", publish_me);
//
//    int num_stats = data_msg.ExtensionSize( micromodem::protobuf::receive_stat );
//    if ( num_stats == 1 ) {
//            publishReceivedInfo( data_msg, 0 );
//    } else if ( num_stats == 2 ) {
//            publishReceivedInfo( data_msg, 1 );
//    } else {
//            stringstream ss;
//            ss << "Error handling ModemTransmission - contained " << num_stats << "statistics.";
//            publishWarning( ss.str() );
//    }
//}

//void acomms_driver_sim::handle_raw_incoming( const goby::acomms::protobuf::ModemRaw& msg ) {
//	string descriptor = msg.raw().substr(3,3);
//		if ( descriptor == "TXF") {
//	//		cout << "transmission finished" << endl;
//			driver_ready = true;
//			publishStatus("ready");
//		} else if ( descriptor == "RXP" ) {
//			driver_ready = false;
//			CST_received = false;
//			RXD_received = false;
//			publishStatus("receiving");
//			receive_set_time = MOOSTime();
//		} else if ( descriptor == "CST" ) {
//			CST_received = true;
//		} else if ( descriptor == "RXD" ) {
//			RXD_received = true;
//		}
//	//	} else {
//	//		cout << "unhandled raw msg with descriptor " << descriptor << endl;
//	//	}
//		if ( !driver_ready && CST_received && RXD_received ) {
//			CST_received = false;
//			RXD_received = false;
//			driver_ready = true;
//			publishStatus("ready");
//		}
//}

//void acomms_driver_sim::publishReceivedInfo( goby::acomms::protobuf::ModemTransmission trans, int index ) {
//	// get statistics out of the transmission
//		micromodem::protobuf::ReceiveStatistics stat = trans.GetExtension( micromodem::protobuf::receive_stat, index );
//
//		lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
//		if ( stat.psk_error_code() == 2 ) {
//			receive_info.num_frames = stat.number_frames();
//			receive_info.num_bad_frames = receive_info.num_frames;
//			receive_info.num_good_frames = 0;
//		} else {
//			receive_info.num_frames = stat.number_frames();
//			receive_info.num_bad_frames = stat.number_bad_frames();
//			receive_info.num_good_frames = receive_info.num_frames - receive_info.num_bad_frames;
//		}
//		receive_info.vehicle_name = my_name;
//		receive_info.source = stat.source();
//		receive_info.rate = stat.rate();
//		m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());
//
//		m_Comms.Notify("ACOMMS_SNR_OUT", stat.snr_out());
//		m_Comms.Notify("ACOMMS_SNR_IN",stat.snr_in());
//		m_Comms.Notify("ACOMMS_DQR",stat.data_quality_factor());
//
//		if ( trans.frame_size() > 0 ) {
//			string frame_string = trans.frame(0);
//			for ( int i=1; i<trans.frame_size(); i++ ) {
//				frame_string += trans.frame(i);
//			}
//			m_Comms.Notify("ACOMMS_RECEIVED_DATA", frame_string);
//		}
//}

void acomms_driver_sim::publishWarning( std::string message ) {
	m_Comms.Notify("ACOMMS_DRIVER_WARNING", message );
}

void acomms_driver_sim::publishStatus( std::string status_update ) {
	status = status_update;
	m_Comms.Notify("ACOMMS_DRIVER_STATUS", status_update );
	status_set_time = MOOSTime();
}

void acomms_driver_sim::startDriver( std::string logDirectory ) {
//	cout << "opening log file: " << logDirectory+"/goby_log.txt" << endl;
//	verbose_log.open((logDirectory+"/goby_log.txt").c_str());
//
//	cfg.set_serial_port( port_name );
//	cfg.set_modem_id( my_id );
//
//	goby::glog.set_name( "iacomms_driver" );
//	goby::glog.add_stream( goby::common::logger::DEBUG1, &std::clog );
//	goby::glog.add_stream( goby::common::logger::DEBUG3, &verbose_log );
//
//	std::cout << "Starting WHOI Micro-Modem MMDriver" << std::endl;
//	driver = new goby::acomms::MMDriver;
//	// turn data quality factor message on
//	// (example of setting NVRAM configuration)
//	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DQF,1");
//	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "MFD,1");
//	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "SHF,1");
//	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "DOP,1");
//	cfg.AddExtension(micromodem::protobuf::Config::nvram_cfg, "IRE,1");
//
//	goby::acomms::connect( &driver->signal_receive, boost::bind(&acomms_driver_sim::handle_data_receive, this, _1) );
//	goby::acomms::connect( &driver->signal_raw_incoming, boost::bind(&acomms_driver_sim::handle_raw_incoming, this, _1) );
//
//	driver->startup(cfg);
	driver_ready = true;
	driver_initialized = true;

	publishStatus("ready");
}
