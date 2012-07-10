/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver_sim.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "acomms_driver_sim.h"

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
	sending_time = 1;
	channel_delay = 3;

	receiving = false;
	transmitting = false;

	transmission_pulse_range = 50;
	transmission_pulse_duration = 5;

	receive_pulse_range = 20;
	receive_pulse_duration = 3;

	srand((unsigned) time(NULL));
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
			//startDriver( msg.GetString() );
		}
		else if ( key == "NAV_X" ) {
		    	  m_navx = msg.GetDouble();
		      } else if ( key == "NAV_Y" ) {
		    	  m_navy = msg.GetDouble();
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

	startDriver("");
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
			//	cout<<"channel delay"<<endl;
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

		ss<<"1,"; // num_frames
		int data_size = transmission_data.size();
		//cout<<"Size: "<<data_size<<endl;
		num_frames = 1;
		if ( data_size > 32 ) {
			transmission_data=transmission_data.substr(0,32);
			//cout<<transmission_data<<endl;
			ss<<transmission_data;
		} else {
			ss<<transmission_data.data();
		}
		transmission_data = "";
	}

	else if ( transmission_rate == 2 ) {
		stringstream psk_ss;
		for ( int i=0; i<3; i++ ) {
			int data_size = transmission_data.size();
			if ( data_size > 64 ) {
				psk_ss.write(transmission_data.data(),64);
				transmission_data = transmission_data.substr( 64, data_size-64 );
				if(i!=2){psk_ss<<":";}
				num_frames++;
			} else if ( data_size > 0 ) {
				num_frames++;
				psk_ss<<transmission_data.data();
				break;
			}
		}
		ss<<num_frames<<",";
		ss<<psk_ss.str();
		transmission_data = "";
	}

	else if ( transmission_rate == 100 ) {
			// if mini packet, just take up to the first two bytes
			// truncating not done
		ss<<"1,";
			int data_size = transmission_data.size();
			if ( data_size > 2 ) // max of 2 bytes ( 13 bits actually )
				data_size = 2;
			ss.write( transmission_data.data(), data_size );
	}

	publishStatus("transmitting");
	driver_ready = false;
	start_time = MOOSTime();

	while(MOOSTime()-start_time<sending_time){
		//wait
		//	cout<<"sending time"<<endl;
	}

	ss.flush();
	//x,y,name,rate,src id,dest id,data, num-frames
	m_Comms.Notify("ACOMMS_SIM_SENT_DATA_LOCAL",ss.str());
	ss.str("");

	driver_ready = true;
	publishStatus("ready");

    postRangePulse( "transmit", transmission_pulse_range, transmission_pulse_duration );

	lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info;
	transmit_info.vehicle_name = my_name;
	transmit_info.rate = transmission_rate;
	transmit_info.dest = transmission_dest;
	transmit_info.num_frames = num_frames;

	m_Comms.Notify("ACOMMS_TRANSMIT_SIMPLE", transmit_info.serializeToString());
}

void acomms_driver_sim::postRangePulse( string label, double range, double duration, string color ) {
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

void acomms_driver_sim::handle_data_receive(string sent_data){
	goby::acomms::protobuf::ModemTransmission data_msg; // changed to not be pointer
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
			num_frames = atoi(substrings[6].c_str());
			data = substrings[7];
			cout<<"Sent from: "<<sender <<" at "<< srcx<<","<<srcy<<endl;
			cout<<"rate: "<<rate<<" num_frames: "<<num_frames<<endl;
			cout << data << endl;
			parsed = true;
		}
		else{
			cout<<"Parsing error."<<endl;
			cout<<"Sent data: "<<sent_data<<endl;
		}
	}
	else{
		cout<<"No sent data"<<endl;
	}

	vector<string> my_frames;
	if(rate==0){
		cout<<"about to add stats" << endl;
		cst_mini = data_msg.AddExtension(micromodem::protobuf::receive_stat);
		cout << "added mini stats" << endl;
		cst = data_msg.AddExtension(micromodem::protobuf::receive_stat);
		cout << "added fsk stats" << endl;
		my_frames.push_back(data);
		cout << "added frame" << endl;
	}
	else if(rate==2){
		cst = data_msg.AddExtension(micromodem::protobuf::receive_stat);
		if(data.size()>0){
			int pos = 0;
			while ( data.find(":", pos) != string::npos ) {
				int newpos = data.find(":", pos);
				string temp_sub = data.substr(pos, newpos-pos);
				cout << "Found: "<<temp_sub<<endl;
				my_frames.push_back(temp_sub);
				pos = newpos+1;
			}

			string temp_sub = data.substr(pos,sent_data.size()-pos);
			my_frames.push_back(temp_sub);
			cout << "Found: "<<temp_sub<<endl;
		}
		if(my_frames.size()<num_frames){
			parsed = false;
			cout << "PSK: only parsed "<<my_frames.size()<<" frames."<<endl;
		}
	}
	else if(rate==100){
		cst = data_msg.AddExtension(micromodem::protobuf::receive_stat);
		cout << "added mini stats" << endl;
		my_frames.push_back(data);
		cout << "added frame" << endl;
	}

	if(parsed){
		vector<double> probs = getProbabilities(x,y,srcx,srcy,rate);
		cout<<"Rolling dice"<<endl;

		if(rollDice(probs[0])){ //sync
			int num_bad_frames = 0;
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
					cst->set_psk_error_code( goby::util::as<micromodem::protobuf::PSKErrorCode>(1) );
				}

				for(int i=0;i<num_frames;i++){
					if(rollDice(probs[3])){//frame
						data_msg.add_frame(my_frames[i]);
					}
					else{ //frame loss
						data_msg.add_frame("");

						cst->set_psk_error_code( goby::util::as<micromodem::protobuf::PSKErrorCode>(3) );
						num_bad_frames++;
					}
				}

			}
			else{	//header loss
				//get nothing, reporting empty
				cout<<"header loss"<<endl;
				cst->set_psk_error_code( goby::util::as<micromodem::protobuf::PSKErrorCode>(2) );
			}

			cst->set_number_bad_frames(num_bad_frames);

			string publish_me = data_msg.DebugString();
			while (publish_me.find("\n") != string::npos) {
				publish_me.replace(publish_me.find("\n"), 1, "<|>");
			}
			m_Comms.Notify("ACOMMS_RECEIVED_ALL", publish_me);

			// file simplified receive info
			lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
			if ( cst->psk_error_code() == 2 ) {
				receive_info.num_frames = cst->number_frames();
				receive_info.num_bad_frames = receive_info.num_frames;
				receive_info.num_good_frames = 0;
			} else {
				receive_info.num_frames = cst->number_frames();
				receive_info.num_bad_frames = cst->number_bad_frames();
				receive_info.num_good_frames = receive_info.num_frames - receive_info.num_bad_frames;
			}
			receive_info.vehicle_name = my_name;
			receive_info.source = cst->source();

			m_Comms.Notify("ACOMMS_RECEIVED_SIMPLE", receive_info.serializeToString());
			m_Comms.Notify("ACOMMS_SNR_OUT", cst->snr_out());
			m_Comms.Notify("ACOMMS_SNR_IN",cst->snr_in());
			m_Comms.Notify("ACOMMS_DQR",cst->data_quality_factor());

			if ( data_msg.frame_size() > 0 ) {
				string frame_string = data_msg.frame(0);
				for ( int i=1; i<data_msg.frame_size(); i++ ) {
					frame_string += data_msg.frame(i);
				}
				m_Comms.Notify("ACOMMS_RECEIVED_DATA", frame_string);
			}

		}
		else{
			//sync loss - reporting nothing
			cout<<"sync loss"<<endl;
		}
	}
}

bool acomms_driver_sim::rollDice(double p){
	int face = rand() % 100;
	if(face<=p){
		cout<<face<<endl;
		cout<<"pass"<<endl;
		return true;
	}
	else{
		cout<<face<<endl;
		cout<<"fail"<<endl;
		return false;
	}

}

vector<double> acomms_driver_sim::getProbabilities(double myx, double myy, double xin, double yin, int rate)
{
	vector<double> probabilities;
	double sync,header,modulation,frame;

	if(rate==100){
		sync = 100;
		header = 100;
		modulation = 100;
		frame = 100;
	}

	else{
	// X-Y Dependent

	double limit = 700;
	double scale = 3;

	double x = scale*(myx+xin)/2/limit;
	double y = scale*(myy+yin)/2/limit;

	sync = 0.5*abs(3 * std::pow((1-x),2) * std::exp(-(std::pow(x,2)) - std::pow((y+1),2))
	   - 9 * (x/5 - std::pow(x,2) - std::pow(y,5)) * std::exp(-pow(x,2)-pow(y,2))
	   - 1/3 * std::exp(-pow((x+1),2) - pow(y,2))) + 7;

	sync=sync*10;
	std::cout << std::endl;
	std::cout << x << "," << y << std::endl;
	std::cout << sync << std::endl;

	header = 99;
	modulation = 99;
	frame = 95;
	}

	probabilities.push_back(sync);
	probabilities.push_back(header);
	probabilities.push_back(modulation);
	probabilities.push_back(frame);
	return probabilities;
}

void acomms_driver_sim::publishWarning( std::string message ) {
	m_Comms.Notify("ACOMMS_DRIVER_WARNING", message );
}

void acomms_driver_sim::publishStatus( std::string status_update ) {
	status = status_update;
	m_Comms.Notify("ACOMMS_DRIVER_STATUS", status_update );
	status_set_time = MOOSTime();
	cout << status_update << endl;
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
