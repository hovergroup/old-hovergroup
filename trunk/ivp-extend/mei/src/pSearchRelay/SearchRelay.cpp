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
	fudge_factor = 10; 	//m outer diameter
	station_factor = 3; //m inner diameter
	min_obs = 10;
	discount = 5;
	num_lookback = 1;
	wait_time = 20; //s

	length = 0;

	action = "paused";
	relay_mode = "default";

	connected = 0;
	end_thrust = 0;

	//Update variables
	update_time = 7;
	last_update = 0;
	voltage = 0;
	srand (time(NULL));
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
		string key = msg.GetKey();

		if(key=="NAV_X"){
			myx = msg.GetDouble();
		}
		else if(key=="NAV_Y"){
			myy = msg.GetDouble();
		}
		else if(key=="DESIRED_THRUST"){
			mythrust = msg.GetDouble();
		}
		else if(key=="SEARCH_RELAY_WAIT_TIME"){
			wait_time = msg.GetDouble();
		}
		else if(key=="ACOMMS_RECEIVED_DATA"){
			if(msg.GetString() != "reset"){

				if(action == "ticking"){
					mail = msg.GetString();

					if(mail.size()!=length){
						action = "compute_failure";
					}
					else{
						action = "relay";
					}
				}
				else{
					cout << "Heard erroneous message: " << msg.GetString() << endl;
				}
			}
		}
		else if(key=="START_TRANSMITTED"){
			if(msg.GetString()!="reset"){
				if(msg.GetString() == "false"){
					action = "start_transmit_now";
				}
				else if(msg.GetString() == "true"){
					action = "sync_with_start";
				}
			}
		}

		else if(key=="END_STATUS"){
			end_status = msg.GetString();
		}

		else if(key=="END_THRUST"){
			end_thrust = msg.GetDouble();
		}

		else if(key=="RELAY_SUCCESS"){
			if(msg.GetString()!="reset"){
				if(msg.GetString()=="true"){
					action = "compute_success";
				}
				else{
					action = "compute_failure";
				}
			}
		}

		//RELAY MAINTENANCE
		else if(key=="SEARCH_RELAY_GOTO_POINT"){
			if(msg.GetDouble() != 99){
				int targind = (int) msg.GetDouble();
				targetx = wpx[targind];
				targety = wpy[targind];

				stringstream ss;
				ss<<"points="<<targetx<<","<<targety;
				cout<<"Updating GOTO Point: "<<ss.str()<<endl;
				m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());

				ss.str("");
				ss<<"station_pt="<<targetx<<","<<targety;
				m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
				m_Comms.Notify("RELAY_MODE","GOTO");
			}
		}

		else if(key=="RELAY_PAUSE"){
			if(msg.GetString() != "reset"){
				if(msg.GetString() == "false"){
					action = "start_transmit_now";
				}
				else{
					action = "paused";
				}
			}
		}
		else if(key=="VOLTAGE")	{
			voltage = msg.GetDouble();
		}
		else if(key=="RELAY_MODE"){
			relay_mode = msg.GetString();
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SearchRelay::OnConnectToServer()
{
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_Comms.Notify("ACOMMS_RECEIVED_DATA","reset");
	m_Comms.Notify("SEARCH_RELAY_GOTO_POINT",99);
	m_Comms.Notify("RELAY_PAUSE","reset");
	m_Comms.Notify("START_TRANSMITTED","reset");
	m_Comms.Notify("RELAY_SUCCESS","reset");

	m_MissionReader.GetConfigurationParam("Mode",mode);
	m_MissionReader.GetConfigurationParam("Discount",discount);
	m_MissionReader.GetConfigurationParam("MinObs",min_obs);
	m_MissionReader.GetConfigurationParam("FudgeFactor",fudge_factor);
	m_MissionReader.GetConfigurationParam("Lookback",num_lookback);
	m_MissionReader.GetConfigurationParam("Rate",rate);

	switch(rate){
	case 0:
		length = 32;
		break;
	case 1:
		length = 192;
		break;
	case 2:
		length = 192;
		break;
	case 3:
		length = 512;
		break;
	case 4:
		length = 512;
		break;
	case 5:
		length = 2048;
		break;
	case 6:
		length = 192;
		break;
	}

	m_Comms.Notify("ACOMMS_TRANSMIT_RATE",rate);

	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
	m_Comms.Register("ACOMMS_RECEIVED_DATA",0);
	m_Comms.Register("SEARCH_RELAY_GOTO_POINT",0);
	m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
	m_Comms.Register("RELAY_PAUSE",0);
	m_Comms.Register("DESIRED_THRUST",0);
	m_Comms.Register("VOLTAGE",0);
	m_Comms.Register("RELAY_MODE",0);

	m_Comms.Register("START_TRANSMITTED",0);
	m_Comms.Register("END_STATUS",0);
	m_Comms.Register("END_THRUST",0);
	m_Comms.Register("RELAY_SUCCESS",0);

	if(mode=="normal"){
		if(discount==5){
			double temp_normal_indices_five[18] = {10.141,1.1656,0.6193,0.4478,0.359,0.3035,0.2645,
					0.2353,0.2123,0.1109,0.0761,0.0582,0.0472,0.0397,0.0343,0.0302,0.0269,0.0244};
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
	}

	connected++;
	cout << "Connected to server "<<connected<<" times"<<endl;

	m_Comms.Notify("RELAY_MODE","GOTO");
	m_Comms.Notify("MISSION_MODE","RELAY");

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SearchRelay::Iterate()
{
	//Initialize
	if(connected==1){
		GetWaypoints();
		targetx = wpx[0];
		targety = wpy[0];

		stringstream ss;
		ss<<"points="<<targetx<<","<<targety;
		m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());

		ss.str("");
		ss<<"station_pt="<<targetx<<","<<targety;
		m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());

		connected++;
	}

	//Thruster control
	double closest_dist = sqrt(pow((targetx-myx),2) + pow((targety-myy),2));

	if(closest_dist < station_factor){
		if(mythrust != 0){
			cout << "Turning thruster off" << endl;
			m_Comms.Notify("MOOS_MANUAL_OVERRIDE","true");
			m_Comms.Notify("RELAY_MODE","KEEP");
		}
	}
	else if(closest_dist > fudge_factor){
		if(relay_mode == "GOTO"){
			if(mythrust == 0){
				cout << "Turning thruster on" << endl;
				m_Comms.Notify("MOOS_MANUAL_OVERRIDE","false");
			}
		}
		else if(relay_mode == "KEEP"){
			if(mythrust == 0 && action!="ticking"){
				cout << "Turning thruster on" << endl;
				m_Comms.Notify("MOOS_MANUAL_OVERRIDE","false");
			}
		}
	}

	//Acoustics
	if(action == "ticking"){
		double time_elapsed = MOOSTime() - start_time;
		if(time_elapsed > wait_time){
			if(relay_mode=="KEEP"){ComputeSuccessRates(0);}
			cout << "Missed sync" << endl << endl;
			action = "start_transmit_now";
		}
	}
	else if(action == "start_transmit_now"){
		m_Comms.Notify("START_TRANSMIT_NOW","true");
		start_time = MOOSTime();
		action = "waiting";
	}
	else if(action == "sync_with_start"){
		start_time = MOOSTime();
		action = "ticking";
	}
	else if(action == "waiting"){
		double time_elapsed = MOOSTime() - start_time;
		if(time_elapsed > wait_time){
			cout << "Missed Wifi Sync with Start" << endl;
			action = "start_transmit_now";
		}
	}
	else if(action == "relay"){
		if(end_thrust == 0){
			if(end_status=="ready"){
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
				action = "ticking";
				start_time = MOOSTime();
			}
			else{
				cout << "Waiting for end driver" << endl;
			}
		}
		else{
			cout << "Waiting for end to stop moving" << endl;
		}
	}
	else if(action == "compute_success"){
		if(relay_mode=="KEEP"){ComputeSuccessRates(1);}
		action = "start_transmit_now";
	}
	else if(action == "compute_failure"){
		if(relay_mode=="KEEP"){ComputeSuccessRates(0);}
		action = "start_transmit_now";
	}

	//Update me!
	if(MOOSTime() - last_update >= update_time){
		stringstream print_me;
		print_me << "Action: " << action << endl;
		print_me << "Distance: " << closest_dist << endl;
		print_me << "Thrust: " << mythrust << endl;
		print_me << "Voltage: " << voltage << endl;
		cout << print_me.str() << endl;
		cout << endl;
		last_update = MOOSTime();
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

void SearchRelay::ComputeSuccessRates(int packet_got){

	int closest_ind = closest_vertex(myx,myy);
	double closest_dist = sqrt(pow((seglist.get_vx(closest_ind)-myx),2) + pow((seglist.get_vy(closest_ind)-myy),2));

	cout<<" Closest distance was: "<< closest_dist << endl;
	cout<<" Closest point was: "<< seglist.get_vx(closest_ind) << " , " << seglist.get_vy(closest_ind)<<std::endl;

	if(closest_dist<=fudge_factor){

		data[closest_ind].push_back(packet_got);

		cout << "Transmissions sent: " << data[closest_ind].size() << endl;
		cout<<"Number of Observations: "<<data[closest_ind].size()/num_lookback<<endl;

		if(data[closest_ind].size() < num_lookback || data[closest_ind].size()/num_lookback < min_obs/num_lookback || data[closest_ind].size()%num_lookback != 0){
			std::cout<<"--->Holding Station"<<std::endl<<std::endl;
		}
		else{
			cout<<"--->Computing Standard Deviation"<<endl;
			int num_obs = data[closest_ind].size()/num_lookback;
			vector<double> means_of_observations (num_obs,-1);
			int sd_counting_index = data[closest_ind].size();

			for(int i=0;i<num_obs;i++){
				double one_mean;
				one_mean = gsl_stats_mean(&(data[closest_ind][sd_counting_index-num_lookback]),1,num_lookback);
				means_of_observations[i] = one_mean;
				cout << "One mean: " << one_mean << endl;
				sd_counting_index = sd_counting_index - num_lookback;
			}

			stdev[closest_ind] = gsl_stats_sd(&(means_of_observations[0]),1,means_of_observations.size());
			mean[closest_ind] = gsl_stats_mean(&(data[closest_ind][0]),1,data[closest_ind].size());

			cout<<"--->Updating data for Point #"<<closest_ind<<std::endl;
			cout<<seglist.get_vx(closest_ind)<<" , "<<seglist.get_vy(closest_ind)<< std::endl;
			cout<<"  Mean:"<<mean[closest_ind]<<" , "<<"STDev:"<< stdev[closest_ind]<< std::endl << endl;

			ComputeIndex(closest_ind);
			cout<<"--->Making a Decision"<<endl;
			int target = Decision();
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

void SearchRelay::ComputeIndex(int closest_ind){
	cout << "--->Computing Index" << endl;
	int num_obs = data[closest_ind].size()/num_lookback;

	if(num_obs==100){
		std::cout<<"  Exceeded maximum observations. Switching Modes."<<std::endl;
		m_Comms.Notify("MISSION_MODE","KEEP");
		std::cout<<"  The final point was: "<<std::endl;
		Decision();
	}

	if(mode=="normal"){
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
	}

	else if(mode=="greedy"){
		indices[closest_ind] = mean[closest_ind];
	}

	else if(mode=="roundrobin"){
		if(closest_ind<total_points-1){
			indices[closest_ind]=0;
			if(indices[closest_ind+1]!=-1){
			indices[closest_ind+1]=1;
			}
		}
		else{
			indices[closest_ind]=0;
			indices[0]=1;
		}
	}

	cout << endl;
	std::cout<<"  Point #"<<closest_ind<<" New Index "<<indices[closest_ind]<<std::endl;

	RelayStat my_stat = RelayStat();
	my_stat.debug_string = "Current";
	my_stat.point_index = closest_ind;
	my_stat.stat_mean = mean[closest_ind];
	my_stat.stat_std=stdev[closest_ind];
	my_stat.gittins_index = indices[closest_ind];
	my_stat.stat_x = myx;
	my_stat.stat_y = myy;
	my_stat.num_obs = num_obs;

	double temp_successes=0;
	for(int i=0;i<data[closest_ind].size();i++){
		temp_successes += data[closest_ind][i];
	}

	my_stat.successful_packets = temp_successes;
	Confess(my_stat);
}

int SearchRelay::Decision(){		//Find largest index
	int target=0;
	stringstream ss;

	for(int i=0;i<indices.size();i++){
		cout<<"Point: "<<i<<" Index: "<<indices[i]<<endl;
		if(mode=="roundrobin"){
			cout<<" Mean: "<<mean[i]<<endl;
		}
		ss<<indices[i];

		if(indices[i]<0){	 // first round
			target = i;
			break;
		}
		else if(indices[i]>indices[target]){
			target = i;
		}
		else if(indices[i]==indices[target]){ //tiebreak

			double new_dist = sqrt(pow((seglist.get_vx(i)-myx),2) + pow((seglist.get_vy(i)-myy),2));
			double old_dist = sqrt(pow((seglist.get_vx(target)-myx),2) + pow((seglist.get_vy(target)-myy),2));

			if(new_dist < old_dist){
				target = i;
			}
		}

		if(i<indices.size()-1){
			ss << "<|>";
		}
	}

	if(mode=="greedy"){
		int temp = rand() % 100 + 1;
		cout<<"Dice Rolled: "<<temp<<endl;
		int target;
		if(temp <= epsilon){
			target = rand()%(total_points-1);
			std::cout<< "Random point was chosen: " << target << std::endl;
		}
	}
	std::cout<<"Decided on Point #"<<target<<" with Index: "<<indices[target]<<std::endl;

	RelayStat my_stat = RelayStat();
	my_stat.debug_string = "Decision";
	my_stat.point_index = target;
	my_stat.stat_mean = mean[target]; my_stat.stat_std=stdev[target];
	my_stat.gittins_index = indices[target];
	my_stat.stat_x = myx;
	my_stat.stat_y = myy;
	my_stat.num_obs = data[target].size()/num_lookback;

	Confess(my_stat);
	m_Comms.Notify("SEARCH_RELAY_INDEX_LIST",ss.str());

	return target;
}
void SearchRelay::Confess(RelayStat stat){
	stringstream ss;
	ss 		<< stat.debug_string <<"<|>"
			<< stat.stat_x << "<|>"
			<< stat.stat_y << "<|>"
			<< stat.point_index << "<|>"
			<< stat.stat_mean << "<|>"
			<< stat.stat_std << "<|>"
			<< stat.num_obs << "<|>"
			<< stat.gittins_index << "<|>"
			<< stat.successful_packets << endl;

	m_Comms.Notify("SEARCH_RELAY_STAT",ss.str());
}

void SearchRelay::GetWaypoints(){ //Waypoints Ordered
	std::string filename = "relay_waypoints.txt";
	std::string one_point;
	std::ifstream waypointsfile("relay_waypoints.txt",std::ifstream::in);
	total_points = 0;

	cout<<"Reading Points"<<endl;

	while(waypointsfile.good()){
		getline(waypointsfile,one_point);
		int pos = one_point.find(',');

		if(pos>0){
			total_points++;
			string subx = one_point.substr(0,pos-1);
			wpx.push_back(atof(subx.c_str()));

			std::string suby = one_point.substr(pos+1);
			wpy.push_back(atof(suby.c_str()));

			seglist.add_vertex(atof(subx.c_str()),atof(suby.c_str()));

			stringstream ss;
			ss<<"type=gateway,x="<<atof(subx.c_str())<<
					",y="<<atof(suby.c_str())<<",SCALE=4.3,label="<<total_points<<",COLOR=green,width=4.5";
			m_Comms.Notify("VIEW_MARKER",ss.str());

			mean.push_back(-1.0);
			stdev.push_back(0.0);
			std::vector<double> temp_vec;
			data[total_points-1] = temp_vec;
			indices.push_back(-1.0);
		}
	}

	std::cout<<"Read "<<total_points<<" points."<<std::endl;
}

//---------------------------------------------------------  MISC

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
