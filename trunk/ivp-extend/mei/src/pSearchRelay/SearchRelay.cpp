/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SearchRelay.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "SearchRelay.h"

//---------------------------------------------------------
// Constructor

SearchRelay::SearchRelay()
{
	normal_indices = std::vector<double> (18, 0);
	fudge_factor = 5; //m
	start = "ready";
	min_obs = 10;	//each link is one obs
	discount = 5;
	rate = 2;
	relay_successful = false;
	num_lookback = 1;
	cumulative_reward = 0;
}

//---------------------------------------------------------
// Destructor

SearchRelay::~SearchRelay()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SearchRelay::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		//all
		//		if(key=="GPS_PTIME"){
		//			now = pt::time_from_string(msg.GetString());
		//			if(my_role=="shore"){
		//				if(last.is_pos_infinity()){last=pt::time_from_string(msg.GetString());}
		//			}
		//		}
		//relay
		if(key=="NAV_X"){
			myx = msg.GetDouble();
		}
		else if(key=="NAV_Y"){
			myy = msg.GetDouble();
		}

		/*		else if(key=="ACOMMS_SNR_OUT"){
			std::cout<<"Stat from "<<msg.GetCommunity()<<std::endl;

			UpdateStats(msg.GetDouble());

			if(relaying&&msg.GetCommunity()=="tech_node"){
				relaying = false;
			}
		}*/

		else if(key=="ACOMMS_TRANSMIT_SIMPLE"){

			std::cout<<"Transmitting from: "<<msg.GetCommunity()<<std::endl;

			if(msg.GetCommunity()=="shore_node"){
				if(waiting){	//Missed sync with shore node
					std::cout<<" Failure to Hear!"<<std::endl;
					ComputeSuccessRates(false);
				}

				else if(relaying){	//Missed sync with end node
					std::cout<<" Failure to Relay!"<<std::endl;
					ComputeSuccessRates(false);
				}

				std::cout<<"--->Shore Transmitted Something... Waiting..."<<std::endl;

				waiting = true;
				relaying = false;
			}
		}
		else if(key=="ACOMMS_RECEIVED_DATA"){
			//std::cout << msg.GetString() << std::endl;
			if(my_role=="relay"){
				if(waiting){
					std::cout<<"--->Relaying..."<<std::endl;
					relay_message = msg.GetString();
					m_Comms.Notify("ACOMMS_TRANSMIT_DATA",relay_message);

					relaying = true;
					waiting = false;
				}
			}
		}

		else if(key=="TRANSMITTED_DATA"){
			std::cout << "Got from Tech: "<< msg.GetString()<<std::endl;
			if(msg.GetString() == "good"){
				ComputeSuccessRates(true);
				m_Comms.Notify("RELAY_SUCCESSFUL","true");
				waiting = false;
				relaying = false;
			}
			else{
				std::cout << msg.GetString();
			}
		}

		else if(key=="SEARCH_RELAY_GOTO_POINT"){
			int targind = (int) msg.GetDouble();
			targetx = wpx[targind];
			targety = wpy[targind];

			std::stringstream ss;
			ss<<"points="<<targetx<<","<<targety;
			std::cout<<"Updating GOTO Point: "<<ss.str()<<std::endl;
			m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
			m_Comms.Notify("RELAY_MODE","GOTO");
			ss.str("");
			ss<<"station_pt="<<targetx<<","<<targety;
			m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
		}

		//shore
		else if(key=="SEARCH_RELAY_WAIT_TIME"){
			wait_time = msg.GetDouble();
			std::cout<<"Setting wait time: "<<wait_time<<std::endl;
		}
		else if(key=="RELAY_STATUS"){
			relay_status = msg.GetString();
			std::cout<<"Heard relay status: "<<relay_status<<std::endl;
		}
		else if(key=="END_STATUS"){
			end_status = msg.GetString();
			std::cout<<"Heard end status: "<<end_status<<std::endl;
		}
		else if(key=="ACOMMS_DRIVER_STATUS"){
			acomms_driver_status = msg.GetString();
		}
		else if(key=="SEARCH_RELAY_START"){
			start = msg.GetString();
		}
		else if(key=="RELAY_SUCCESSFUL"){
			if(msg.GetString()=="true"){
				relay_successful = true;
			}
		}
		//end
		else if(key=="ACOMMS_RECEIVED_SIMPLE"){
			lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info(msg.GetString());
			if(receive_info.source==relay_id && receive_info.num_good_frames==receive_info.num_frames){
				m_Comms.Notify("TRANSMITTED_DATA","good");
			}
			else{
				std::stringstream ss;
				ss << "Lost "<<receive_info.num_bad_frames<<" number of frames."<<std::endl;
				m_Comms.Notify("TRANSMITTED_DATA",ss.str());
			}
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SearchRelay::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_MissionReader.GetConfigurationParam("Role",my_role);

	if(my_role=="relay"){
		m_MissionReader.GetConfigurationParam("Mode",mode);
		m_MissionReader.GetConfigurationParam("Discount",discount);
		m_MissionReader.GetConfigurationParam("MinObs",min_obs);
		m_MissionReader.GetConfigurationParam("FudgeFactor",fudge_factor);
		m_MissionReader.GetConfigurationParam("Lookback",num_lookback);

		m_Comms.Register("NAV_X",0);
		m_Comms.Register("NAV_Y",0);
		m_Comms.Register("ACOMMS_RECEIVED_DATA",0);
		m_Comms.Register("SEARCH_RELAY_GOTO_POINT",0);
		m_Comms.Register("ACOMMS_TRANSMIT_SIMPLE",0);
		m_Comms.Register("TRANSMITTED_DATA",0);

		if(mode=="normal"){
			if(discount==5){

				double temp_normal_indices_five[18] = {10.141,1.1656,0.6193,0.4478,0.359,0.3035,0.2645,
						0.2353,0.2123,0.1109,0.0761,0.0582,0.0472,0.0397,0.0343,0.0302,0.0269,0.0244};
				//std::cout << "memcpy " << sizeof(temp_normal_indices_five[0]) << std::endl;
				memcpy( &normal_indices[0], &temp_normal_indices_five[0], sizeof(temp_normal_indices_five[0])*18 );
			}
			else if(discount==1){
				double temp_normal_indices_one[18] = {39.3343,3.102,1.3428,0.9052,0.7054,0.5901,0.5123,0.4556,0.4119,
						0.223,0.1579,0.1235,0.1019,0.087,0.076,0.0675,0.0608,0.0554};
				memcpy( &normal_indices[0], &temp_normal_indices_one[0], sizeof(temp_normal_indices_one[0])*18 );
			}
		}
		else if(mode == "greedy"){
			m_MissionReader.GetConfigurationParam("Epsilon",epsilon);
			srand((unsigned) time(NULL));
		}

		waiting = false;
		relaying = false;
		GetWaypoints();
		targetx = wpx[0];
		targety = wpy[0];

		std::stringstream ss;
		ss<<"points="<<targetx<<","<<targety;
		m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
		ss.str("");
		ss<<"station_pt="<<targetx<<","<<targety;
		m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
		m_Comms.Notify("RELAY_MODE","GOTO");
		m_Comms.Notify("MISSION_MODE","RELAY");
		m_Comms.Notify("ACOMMS_TRANSMIT_RATE",2);
		m_Comms.Notify("RELAY_STATUS","ready");
	}

	else if(my_role=="end"){
		m_MissionReader.GetConfigurationParam("RelayID",relay_id);
		m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
		m_Comms.Notify("END_STATUS","ready");
	}

	else if(my_role=="shore"){

		m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
		m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
		m_Comms.Register("RELAY_STATUS",0);
		m_Comms.Register("END_STATUS",0);
		m_Comms.Register("SEARCH_RELAY_START",0);
		m_Comms.Register("RELAY_SUCCESSFUL",0);
		wait_time = 15; //s
		rate = 2;
		m_Comms.Notify("ACOMMS_TRANSMIT_RATE",rate);
		last = 0;
		m_MissionReader.GetConfigurationParam("WaitTime",wait_time);
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SearchRelay::Iterate()
{
	// happens AppTick times per second

	if(my_role=="relay"){
	}

	else if(my_role=="end"){ //do nothing

	}

	else if(my_role=="shore" && relay_status=="ready" && end_status=="ready" && start=="ready" && acomms_driver_status=="ready"){
		//transmit as soon as possible
		double temp = MOOSTime() - last;
		std::cout<<"Last: "<< temp <<std::endl;
		if( (MOOSTime()-last>=wait_time) || (relay_successful)){

			if(relay_successful){std::cout << "Relay Successful" << std::endl;}
			relay_successful = false;

			int length;
			std::stringstream ss;
			ss << counter;

			if(rate==0){length = 32;}
			else if(rate==2){length = 192;}

			std::string mail = ss.str()+"---"+getRandomString(length);
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
			last = MOOSTime();
			counter++;
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SearchRelay::OnStartUp()
{
	// happens before connection is open

	return(true);
}

//---------------------------------------------------------
// Search Relay Methods

void SearchRelay::ComputeIndex(int closest_ind){
	std::cout<<"---------->Computing Index"<<std::endl;
	int num_obs = data[closest_ind].size()/num_lookback;
	std::cout<<"  Num Obs: "<<num_obs<<std::endl;

	if(num_obs==100){
		std::cout<<"  Exceeded maximum observations. Switching Modes."<<std::endl;
		m_Comms.Notify("MISSION_MODE","KEEP");
		m_Comms.Notify("RELAY_STATUS","Completed");
		std::cout<<"  The final point was: "<<std::endl;
		Decision();
	}

	double gindex;
	if(num_obs<=10){
		gindex = normal_indices[num_obs-2];//table starts from 2 obs, vector starts from 0 index
		std::cout<<"   Observations: "<<num_obs<<", gindex: "<<gindex<<std::endl;
	}
	else{	//interpolate
		int base = 7+(num_obs/10);
		int offset = num_obs%10;
		double difference = (normal_indices[base+1]-normal_indices[base])/10;
		gindex = normal_indices[base] + offset*difference;
		std::cout<<"   Observations: "<<" ,gindex: "<<gindex<<std::endl;
	}

	indices[closest_ind] = mean[closest_ind]+stdev[closest_ind]*gindex;
	std::cout<<"  Point #"<<closest_ind<<" New Index "<<indices[closest_ind]<<std::endl;

	int target = closest_ind;
	RelayStat my_stat = RelayStat();
	my_stat.debug_string = "Compute";
	my_stat.x = myx; my_stat.y = myy;
	my_stat.next_x = wpx[target]; my_stat.next_y = wpy[target];
	my_stat.stat_mean = mean[target]; my_stat.stat_std=stdev[target];
	my_stat.index = indices[target];

	double temp_successes=0;
	for(int i=0;i<data[closest_ind].size();i++){
		temp_successes += data[closest_ind][i];
	}

	my_stat.successful_packets = temp_successes;

	Confess(my_stat);
}

int SearchRelay::Decision(){
	int target=0;
	std::stringstream ss;
	bool complete = false;

	if(mode == "normal"){
		//Find largest index

		for(int i=0;i<indices.size();i++){
			std::cout<<"Point: "<<i<<" Index: "<<indices[i]<<std::endl;

			if(indices[i]<0){
				target = i;
				complete = false;
				break;
			}

			else if(indices[i]>indices[target]){
				target = i;
			}
			ss<<indices[i]<<",";
			complete = true;
		}

		std::cout<<"Decided on Point #"<<target<<" with Index: "<<indices[target]<<std::endl;

		RelayStat my_stat = RelayStat();
		my_stat.debug_string = "Decision";
		my_stat.x = myx; my_stat.y = myy;
		my_stat.next_x = wpx[target]; my_stat.next_y = wpy[target];
		my_stat.stat_mean = mean[target]; my_stat.stat_std=stdev[target];
		my_stat.index = indices[target];

		Confess(my_stat);
		if(complete){
			m_Comms.Notify("SEARCH_RELAY_INDEX_LIST",ss.str());
		}
	}

	else if(mode == "greedy"){
		for(int i=0;i<mean.size();i++){
			std::cout<<"Point: "<<i<<" Mean: "<<mean[i]<<std::endl;

			if(mean[i]<0){
				target = i;
				complete = false;
				break;
			}

			else if(mean[i]>mean[target]){
				target = i;
			}
			ss<<mean[i]<<",";
			complete = true;
		}
		std::cout<<"Decided on Point #"<<target<<" with Mean: "<<mean[target]<<std::endl;
		if(complete){
			m_Comms.Notify("SEARCH_RELAY_MEAN_LIST",ss.str());
		}
	}

	return target;
}

void SearchRelay::ComputeSuccessRates(bool packet_got){
	//Here assuming last nav variables is current position
	//Update the mean and var of one node if current position is within
	//a fudge factor of that node
	int closest_ind = closest_vertex(myx,myy);
	double closest_dist = sqrt(pow((seglist.get_vx(closest_ind)-myx),2) + pow((seglist.get_vy(closest_ind)-myy),2));

	std::cout<<"Trying COMPUTE SUCCESS RATES"<<std::endl;
	std::cout<<" Closest distance was: "<<closest_dist<<std::endl;
	std::cout<<" Closest point was: "<< seglist.get_vx(closest_ind) << " , " << seglist.get_vy(closest_ind)<<std::endl;

	if(closest_dist<=fudge_factor){
		if(packet_got){
			data[closest_ind].push_back(1);
			cumulative_reward += 1;
			m_Comms.Notify("RELAY_REWARD",cumulative_reward);
		}
		else{
			data[closest_ind].push_back(0);
		}

		mean[closest_ind] = gsl_stats_mean(&(data[closest_ind][0]),1,data[closest_ind].size());
		stdev[closest_ind] = gsl_stats_sd(&(data[closest_ind][0]),1,data[closest_ind].size());

		std::cout<<"  Updating MEAN and STDev for Point #"<<closest_ind<<std::endl;
		std::cout<<seglist.get_vx(closest_ind)<<" , "<<seglist.get_vy(closest_ind)<< std::endl;
		std::cout<<"  Mean:"<<mean[closest_ind]<<" , "<<"STDev:"<< stdev[closest_ind]<< std::endl;

		if(mode == "normal"){
			if(data[closest_ind].size() < num_lookback || data[closest_ind].size()/num_lookback < min_obs/num_lookback || data[closest_ind].size()%num_lookback != 0){
				std::cout<<"Number of Observations: "<<data[closest_ind].size()/num_lookback<<std::endl;
				std::cout<<"--->Holding Station"<<std::endl<<std::endl;
			}
			else{
				ComputeIndex(closest_ind);

				int target = Decision();
				std::cout<<"Number of Observations: "<<data[closest_ind].size()/num_lookback<<std::endl;
				std::cout<<"--->Making a Decision"<<std::endl;
				targetx = wpx[target];
				targety = wpy[target];
				std::stringstream ss;
				ss<<"points="<<myx<<","<<myy<<":"<<targetx<<","<<targety;
				std::cout<<"Updating: "<<ss.str()<<std::endl<<std::endl;
				m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
				m_Comms.Notify("RELAY_MODE","GOTO");
				ss.str("");
				ss<<"station_pt="<<targetx<<","<<targety;
				m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
			}
		}
		else if(mode == "greedy"){
			if(data[closest_ind].size()<min_obs){
				std::cout<<"Number of Observations: "<<data[closest_ind].size()<<std::endl;
				std::cout<<"--->Holding Station"<<std::endl<<std::endl;
			}
			else{
				std::cout<<"Number of Observations: "<<data[closest_ind].size()/num_lookback<<std::endl;
				std::cout<<"--->Making a Decision"<<std::endl;

				double temp = rand() % 100;
				std::cout<<"Dice Rolled: "<<temp<<std::endl;
				int target;
				if(temp <= epsilon){
					target = rand()%(total_points-1);
					std::cout<< "Random point: " << target << std::endl;
				}
				else{
					target = Decision();
				}


				targetx = wpx[target];
				targety = wpy[target];
				std::stringstream ss;
				ss<<"points="<<myx<<","<<myy<<":"<<targetx<<","<<targety;
				std::cout<<"Updating: "<<ss.str()<<std::endl<<std::endl;
				m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
				m_Comms.Notify("RELAY_MODE","GOTO");
				ss.str("");
				ss<<"station_pt="<<targetx<<","<<targety;
				m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
			}
		}
	}
}

void SearchRelay::Confess(RelayStat stats){
	std::stringstream ss;
	ss << "TYPE: " << stats.debug_string <<"<|>"
			<< "STAT_X: " << stats.x<<"<|>"
			<< "STAT_Y: " << stats.y<<"<|>"
			<< "TARGET_X: " << stats.next_x <<"<|>"
			<<"TARGET_Y: " << stats.next_y <<"<|>"
			<< "MEAN: " << stats.stat_mean <<"<|>"
			<< "STDEV: " << stats.stat_std << "<|>"
			<< "INDEX: "<< stats.index << "<|>"
			<< "SUCCESSES: "<<stats.successful_packets << std::endl;

	m_Comms.Notify("SEARCH_RELAY_STATS",ss.str());
}

void SearchRelay::GetWaypoints(){ //Waypoints Ordered
	std::string filename = "relay_waypoints.txt";
	std::string one_point;
	std::ifstream waypointsfile("relay_waypoints.txt",std::ifstream::in);
	int counter = 1;

	while(waypointsfile.good()){
		getline(waypointsfile,one_point);
		int pos = one_point.find(',');
		std::cout << pos << std::endl;
		if(pos>0){
			std::cout<<"Reading Points"<<std::endl;
			std::cout<<one_point<<std::endl;
			std::cout<<counter<<std::endl;
			std::string subx = one_point.substr(0,pos-1);
			wpx.push_back(atof(subx.c_str()));
			std::cout<<subx<<std::endl;
			std::string suby = one_point.substr(pos+1);
			std::cout<<suby<<std::endl;
			wpy.push_back(atof(suby.c_str()));
			seglist.add_vertex(atof(subx.c_str()),atof(suby.c_str()));

			std::stringstream ss;
			ss<<"type=gateway,x="<<atof(subx.c_str())<<
					",y="<<atof(suby.c_str())<<",SCALE=4.3,label="<<counter<<",COLOR=green,width=4.5";
			m_Comms.Notify("VIEW_MARKER",ss.str());

			mean.push_back(-1.0);
			stdev.push_back(0.0);
			std::vector<double> temp_vec;
			data[counter-1] = temp_vec;
			indices.push_back(-1.0);
			counter++;
		}
	}

	total_points = counter;
	std::cout<<"Read "<<counter<<" points."<<std::endl;
}

std::string SearchRelay::getRandomString( int length ) {
	srand((unsigned) time(NULL));

	std::stringstream ss;
	const int passLen = length;
	for (int i = 0; i < passLen; i++) {
		char num = (char) ( rand() % 62 );
		if ( num < 10 )
			num += '0';
		else if ( num < 36 )
			num += 'A'-10;
		else
			num += 'a'-36;
		ss << num;
	}

	return ss.str();
}

unsigned int SearchRelay::closest_vertex(double x, double y)
{
	unsigned int vsize = seglist.size();

	if(vsize == 0)
		return(0);

	double closest_dist = sqrt(pow((seglist.get_vx(0)-myx),2) + pow((seglist.get_vy(0)-myy),2));

	unsigned int i, ix = 0;
	for(i=1; i<vsize; i++) {
		double idist = sqrt(pow((seglist.get_vx(i)-myx),2) + pow((seglist.get_vy(i)-myy),2));
		if(idist < closest_dist) {
			closest_dist = idist;
			ix = i;
		}
	}
	return(ix);
}

/*
void SearchRelay::UpdateStats(double snr_data){
	//Here assuming last nav variables is current position
	//Update the mean and var of one node if current position is within
	//a fudge factor of that node
	int closest_ind = closest_vertex(myx,myy);
	double closest_dist = sqrt(pow((seglist.get_vx(closest_ind)-myx),2) + pow((seglist.get_vy(closest_ind)-myy),2));
	std::cout<<"Trying UPDATE"<<std::endl;
	std::cout<<" Closest distance was: "<<closest_dist<<std::endl;
	std::cout<<" Closest point was: "<< seglist.get_vx(closest_ind) << " , " << seglist.get_vy(closest_ind)<<std::endl;
	if(closest_dist<fudge_factor){
		data[closest_ind].push_back(snr_data);
		mean[closest_ind] = gsl_stats_mean(&(data[closest_ind][0]),1,data[closest_ind].size());
		stdev[closest_ind] = gsl_stats_sd(&(data[closest_ind][0]),1,data[closest_ind].size());
		std::cout<<"  Updating MEAN and STDev for Point #"<<closest_ind<<std::endl;
		std::cout<<seglist.get_vx(closest_ind)<<" , "<<seglist.get_vy(closest_ind)<< std::endl;
		std::cout<<"  Mean:"<<mean[closest_ind]<<" , "<<"STDev:"<< stdev[closest_ind]<< std::endl;

		std::stringstream ss;
		ss<<"STAT_X:"<<myx<<"<|>"<<"STAT_Y:"<<myy<<"<|>"<<
				"TARGET_X:"<<seglist.get_vx(closest_ind)<<"<|>"<<
				"TARGET_Y:"<<seglist.get_vy(closest_ind)<<"<|>"<<
				"SAMPLE_MEAN:"<<mean[closest_ind]<<"<|>"<<
				"SAMPLE_STDEV:"<<stdev[closest_ind];
		m_Comms.Notify("SEARCH_RELAY_STATS",ss.str());

		if(data[closest_ind].size() < min_obs){
			std::cout<<"Number of Observations: "<<data[closest_ind].size()<<std::endl;
			std::cout<<"--->Holding Station";
		}
		else{
			ComputeIndex();

			if(closest_ind < wpx.size() && data[closest_ind+1].size() < min_obs){ //First pass
				targetx = wpx[closest_ind+1];
				targety = wpy[closest_ind+1];
				std::stringstream ss;
				ss<<"points="<<myx<<","<<myy<<":"<<targetx<<","<<targety;
				std::cout<<"Updating: "<<ss.str()<<std::endl;
				m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
				m_Comms.Notify("RELAY_MODE","GOTO");
				ss.str("");
				ss<<"station_pt="<<targetx<<","<<targety;
				m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
			}
			else{
				int target = Decision();
				std::cout<<"Number of Observations: "<<data[closest_ind].size()<<std::endl;
				std::cout<<"--->Making a Decision";
				targetx = wpx[target];
				targety = wpy[target];
				std::stringstream ss;
				ss<<"points="<<myx<<","<<myy<<":"<<targetx<<","<<targety;
				std::cout<<"Updating: "<<ss.str()<<std::endl;
				m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
				m_Comms.Notify("RELAY_MODE","GOTO");
				ss.str("");
				ss<<"station_pt="<<targetx<<","<<targety;
				m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
			}
		}
	}
}
 */
