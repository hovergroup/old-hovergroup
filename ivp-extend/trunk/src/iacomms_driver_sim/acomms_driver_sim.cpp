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
	sending_time = 3;
	channel_delay = 3;

	receiving = false;
	transmitting = false;
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
		driver_ready=false;
		publishStatus("receiving");
		while(MOOSTime()-sent_time < channel_delay){
			driver_ready = false;
			cout<<"channel delay"<<endl;
		}
		handle_data_receive(sent_data);
		receiving = false;
		driver_ready = true;
		publishStatus("ready");
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

	stringstream ss;
	int num_frames = 0;

	//x,y,name,rate,src id,dest id,data, num-frames
	ss <<x<<","<<y<<","
			<<my_name<<","
			<<transmission_rate<<","
			<<my_id<<","
			<<transmission_dest<<",";

	cout<<"Transmitting: "<<ss.str()<<endl;
	cout<<transmission_data<<endl;

	if ( transmission_rate == 0 ) {
		int data_size = transmission_data.size();
		cout<<"Size: "<<data_size<<endl;
		num_frames = 1;
		if ( data_size > 32 ) {
			transmission_data=transmission_data.substr(0,32);
			cout<<transmission_data<<endl;
			ss<<transmission_data;
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


	publishStatus("transmitting");
	driver_ready = false;
	start_time = MOOSTime();

	while(MOOSTime()-start_time<sending_time){
		//wait
		cout<<"sending time"<<endl;
	}

	//x,y,name,rate,src id,dest id,data, num-frames
	m_Comms.Notify("ACOMMS_SIM_SENT_DATA",ss.str());
	ss.str("");

	driver_ready = true;
	publishStatus("ready");

	//x=100,y=-50,radius=40,duration=15,fill=0.25,
	//fill color=green,label=04,time=@MOOSTIME
	ss<<"x="<<x<<",y="<<y;
	m_Comms.Notify("VIEW_RANGE_PULSE", ss.str());

	lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info;
	transmit_info.vehicle_name = my_name;
	transmit_info.rate = transmission_rate;
	transmit_info.dest = transmission_dest;
	transmit_info.num_frames = num_frames;

	m_Comms.Notify("ACOMMS_TRANSMIT_SIMPLE", transmit_info.serializeToString());
}

void acomms_driver_sim::handle_data_receive(string sent_data){
	goby::acomms::protobuf::ModemTransmission* data_msg;
	micromodem::protobuf::ReceiveStatistics* cst;
	micromodem::protobuf::ReceiveStatistics* cst_mini;

	vector<string> substrings;
	//x,y,name,rate,src id,dest id,data, num-frames
	string sender,data;
	double srcx,srcy;
	int rate,srcid,destid,num_frames;
	bool parsed = false;

	if(sent_data.size()>0){
		int pos = 0;
		while ( sent_data.find(",", pos) != string::npos ) {
			int newpos = sent_data.find(",", pos);
			string temp_sub = sent_data.substr(pos, newpos-pos);
			substrings.push_back(temp_sub);
			pos = newpos+1;
		}

		string temp_sub = sent_data.substr(pos,sent_data.size()-pos);
		substrings.push_back(temp_sub);

		if(substrings.size()>=8){
			srcx = atof(substrings[0].c_str());
			srcy = atof(substrings[1].c_str());
			sender = substrings[2];
			rate = atoi(substrings[3].c_str());
			srcid = atoi(substrings[4].c_str());
			destid = atoi(substrings[5].c_str());
			data = substrings[6];
			num_frames = atoi(substrings[7].c_str());
			cout<<"Sent from: "<<sender <<" at "<< srcx<<","<<srcy<<endl;
			cout<<"rate: "<<rate<<" num_frames: "<<num_frames<<endl;
			parsed = true;
		}
		else{
			cout<<"Parsing error."<<endl;
		}
	}
	else{
		cout<<"No sent data"<<endl;
	}
	cout<<"is here" << endl;

	vector<string> my_frames;
	if(rate==0){
		cout<<"here" << endl;
		cst_mini = data_msg->AddExtension(micromodem::protobuf::receive_stat);
		cst = data_msg->AddExtension(micromodem::protobuf::receive_stat);
		my_frames.push_back(data);
	}
	else if(rate==2){
		cout<<"or here" << endl;
		cst = data_msg->AddExtension(micromodem::protobuf::receive_stat);
		if(data.size()>0){
			int pos = 0;
			while ( data.find(":", pos) != string::npos ) {
				int newpos = data.find(":", pos);
				string temp_sub = sent_data.substr(pos, newpos-pos);
				my_frames.push_back(temp_sub);
				pos = newpos+1;
			}

			string temp_sub = data.substr(pos,sent_data.size()-pos);
			my_frames.push_back(temp_sub);
		}
		if(my_frames.size()<3){
			parsed = false;
			cout << "PSK: only parsed "<<my_frames.size()<<" frames."<<endl;
		}
	}
	cout<<"or or here" << endl;
	if(parsed){
		vector<double> probs = getProbabilities(x,y,srcx,srcy,rate);
		cout<<"Rolling dice"<<endl;

		if(rollDice(probs[0])){ //sync
			if(rollDice(probs[1])){ //header
				if(rollDice(probs[2])){ //modulation
					cst->set_rate(rate);
					cst->set_source(srcid);
					cst->set_dest(destid);
					cst->set_number_frames(num_frames);
				}

				else{ //modulation loss
					//no header
					cout<<"mod loss"<<endl;
					cst->set_psk_error_code(1);
				}

				for(int i=0;i<num_frames;i++){
					if(rollDice(probs[3])){//frame
						data_msg->add_frame(my_frames[i]);
					}
					else{ //frame loss
						data_msg->add_frame("");
						cst->set_psk_error_code(3);
					}
				}

			}
			else{	//header loss
				//get nothing, reporting empty
				cout<<"header loss"<<endl;
				cst->set_psk_error_code(2);
			}

			string publish_me = data_msg->DebugString();
			while (publish_me.find("\n") != string::npos) {
				publish_me.replace(publish_me.find("\n"), 1, "<|>");
			}
			m_Comms.Notify("ACOMMS_RECEIVED_ALL", publish_me);

			lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
			if ( cst->psk_error_code() == 2 ) {
				receive_info.num_frames = num_frames;
				receive_info.num_bad_frames = num_frames;
				receive_info.num_good_frames = 0;
			} else {
				receive_info.num_frames = num_frames;
				receive_info.num_bad_frames = cst->number_bad_frames();
				receive_info.num_good_frames = receive_info.num_frames - receive_info.num_bad_frames;
			}
			receive_info.vehicle_name = my_name;
			receive_info.source = cst->source();
			receive_info.rate = cst->rate();
			m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());

			m_Comms.Notify("ACOMMS_SNR_OUT", cst->snr_out());
			m_Comms.Notify("ACOMMS_SNR_IN",cst->snr_in());
			m_Comms.Notify("ACOMMS_DQR",cst->data_quality_factor());

			m_Comms.Notify("ACOMMS_RECEIVED_DATA", data);

		}
		else{
			//sync loss - reporting nothing
			cout<<"sync loss"<<endl;
		}
	}
}

bool acomms_driver_sim::rollDice(double p){
	srand((unsigned) time(NULL));
	int face = rand() % 100;
	if(face<=p){
		cout<<"pass"<<endl;
		return true;
	}
	else{
		cout<<"fail"<<endl;
		return false;
	}

}

vector<double> acomms_driver_sim::getProbabilities(double myx, double myy, double x, double y, int rate)
{
	vector<double> probabilities;
	if(abs(myx-x) <5 && abs(myy-y)<5){
		cout<<"Really close!"<<endl;
		probabilities.push_back(100);
	}
	else{
	double sync_loss = 20;
	double header_loss = 1;
	double modulation_loss = 5;
	double frame_loss = 5;
	probabilities.push_back(sync_loss);
	probabilities.push_back(header_loss);
	probabilities.push_back(modulation_loss);
	probabilities.push_back(frame_loss);}

	return probabilities;
}

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
