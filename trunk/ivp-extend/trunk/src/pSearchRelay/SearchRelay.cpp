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
	start = "default";
	min_obs = 10;	//each link is one obs
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
		if(key=="GPS_PTIME"){
			now = pt::time_from_string(msg.GetString());
			if(my_role=="shore"){
				if(last.is_pos_infinity()){last=pt::time_from_string(msg.GetString());}
			}
		}
		//relay
		else if(key=="SEARCH_RELAY_RESET"){
			data.clear();
			mean.clear();
			var.clear();
			indices.clear();
			targetx = wpx[0];
			targety = wpy[0];
		}
		else if(key=="NAV_X"){
			myx = msg.GetDouble();
		}
		else if(key=="NAV_Y"){
			myy = msg.GetDouble();
		}
		else if(key=="ACOMMS_SNR_OUT"){
			std::cout<<"Stat from "<<msg.GetCommunity()<<std::endl;

			if(msg.GetCommunity()=="tech_node"){
				transmit_success = true;
				relaying = false;

				bool stats_updated = UpdateStats(msg.GetDouble());

				if(stats_updated){
					stats_updated = false;
					int closest_ind = closest_vertex(myx,myy);
					double closest_dist = sqrt(pow((seglist.get_vx(closest_ind)-myx),2) + pow((seglist.get_vy(closest_ind)-myy),2));

					if(closest_dist<fudge_factor){

						if(data[closest_ind].size() < min_obs){
							//Not enough data, Stay in place
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
								ss.clear();
								ss<<"station_pt="<<targetx<<","<<targety;
								m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
							}
							else{
								int target = Decision();
								targetx = wpx[target];
								targety = wpy[target];
								std::stringstream ss;
								ss<<"points="<<myx<<","<<myy<<":"<<targetx<<","<<targety;
								std::cout<<"Updating: "<<ss.str()<<std::endl;
								m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
								m_Comms.Notify("RELAY_MODE","GOTO");
								ss.clear();
								ss<<"station_pt="<<targetx<<","<<targety;
								m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
							}

						}
					}
				}
			}
			else{
				link1_stat = msg.GetDouble();
				UpdateStats(link1_stat);
			}

		}

		else if(key=="ACOMMS_TRANSMIT_SIMPLE"){
			std::cout<<"Msg from: "<<msg.GetCommunity()<<std::endl;
			if(msg.GetCommunity()=="shore_node"){
				if(waiting){	//Missed sync with shore node
					std::cout<<"Missed Sync with Shore"<<std::endl;
					UpdateStats(0.0);
					UpdateStats(0.0);
				}

				else if(relaying){	//Missed sync with end node
					std::cout<<"Missed Sync with Tech"<<std::endl;
					relaying = false;
					UpdateStats(0.0);
				}

				std::cout<<"Shore Transmitted Something... Waiting..."<<std::endl;

				waiting = true;
				relaying = false;
				transmit_success = false;
			}
		}
		else if(key=="ACOMMS_RECEIVED_DATA"){
			if(waiting){
				std::cout<<"Relaying..."<<std::endl;
				relay_message = msg.GetString();
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA",relay_message);
				relaying = true;
				waiting = false;
			}
		}

		//shore
		else if(key=="SEARCH_RELAY_WAIT_TIME"){
			wait_time = pt::seconds((long) msg.GetDouble());
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

	m_Comms.Register("GPS_PTIME",0);

	if(my_role=="relay"){

		discount = 5;

		m_MissionReader.GetConfigurationParam("Mode",mode);
		m_MissionReader.GetConfigurationParam("Discount",discount);
		m_Comms.Register("ACOMMS_SNR_OUT",0);
		m_Comms.Register("NAV_X",0);
		m_Comms.Register("NAV_Y",0);
		m_Comms.Register("ACOMMS_RECEIVED_DATA",0);
		m_Comms.Register("SEARCH_RELAY_RESET",0);
		m_Comms.Register("ACOMMS_TRANSMIT_SIMPLE",0);

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

		relaying = false;
		//std::cout<<"wpts"<<std::endl;
		GetWaypoints();
		targetx = wpx[0];
		targety = wpy[0];

		std::stringstream ss;
		ss<<"points="<<targetx<<","<<targety;
		//std::cout<<"Updating: "<<ss.str()<<std::endl;
		m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
		m_Comms.Notify("RELAY_MODE","GOTO");
		m_Comms.Notify("MISSION_MODE","RELAY");
		m_Comms.Notify("ACOMMS_TRANSMIT_RATE",2);
		m_Comms.Notify("RELAY_STATUS","ready");
	}

	else if(my_role=="end"){
		m_Comms.Notify("END_STATUS","ready");
	}

	else if(my_role=="shore"){
		m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
		m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
		m_Comms.Register("RELAY_STATUS",0);
		m_Comms.Register("END_STATUS",0);
		m_Comms.Register("SEARCH_RELAY_START",0);
		wait_time = pt::seconds(15);
		rate = 2;
		m_Comms.Notify("ACOMMS_TRANSMIT_RATE",rate);
		last = pt::ptime(pt::pos_infin);
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

	else if(my_role=="shore" && relay_status=="ready" && end_status=="ready" && start=="ready"){ //transmitting every wait_time seconds

		if(now-last>=wait_time && acomms_driver_status=="ready"){
			std::cout<<now<<std::endl;
			std::cout<<last<<std::endl;
			int length;
			std::stringstream ss;
			ss << counter;
			if(rate==0){length = 32;}
			else if(rate==2){length = 192;}
			std::string mail = ss.str()+"---"+getRandomString(length);
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
			counter++;
			last = pt::ptime(now);
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

bool SearchRelay::UpdateStats(double snr_data){
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
		var[closest_ind] = gsl_stats_variance(&(data[closest_ind][0]),1,data[closest_ind].size());
		std::cout<<"  Updating Mean and Var for Point #"<<closest_ind<<std::endl;
		std::cout<<seglist.get_vx(closest_ind)<<" , "<<seglist.get_vy(closest_ind)<< std::endl;
		std::cout<<   "Mean:"<<mean[closest_ind]<<" , "<<"Var:"<<var[closest_ind]<< std::endl;
		return true;
	}
	else{
		return false;
	}
}

void SearchRelay::ComputeIndex(){
	if(mode=="normal"){
		int closest_ind = closest_vertex(myx,myy);
		double closest_dist = sqrt(pow((seglist.get_vx(closest_ind)-myx),2) + pow((seglist.get_vy(closest_ind)-myy),2));
		std::cout<<"Trying COMPUTE INDEX"<<std::endl;
		std::cout<<" Closest distance was: "<<closest_dist<<std::endl;
		std::cout<<" Closest point was: "<< seglist.get_vx(closest_ind) << " , " << seglist.get_vy(closest_ind)<<std::endl;
		if(closest_dist<fudge_factor){
			std::cout<<"  Computing Index..."<<std::endl;
			int num_obs = data[closest_ind].size()/2;
			std::cout<<"  Num Obs (/2) : "<<num_obs<<std::endl;

			if(num_obs==100){
				std::cout<<"  Exceeded maximum observations. Switching Modes."<<std::endl;
				m_Comms.Notify("MISSION_MODE","KEEP");
				m_Comms.Notify("RELAY_STATUS","Completed");
			}

			double gindex;
			if(num_obs<=10){
				gindex = normal_indices[num_obs-2];//table starts from 2 obs, vector starts from 0 index
				std::cout<<"  1-10 gindex: "<<gindex<<std::endl;
			}
			else{	//interpolate
				int base = 7+(num_obs/10);
				int offset = num_obs%10;
				double difference = (normal_indices[base]-normal_indices[base+1])/10;
				gindex = normal_indices[base] + offset*difference;
				std::cout<<"  10-100 gindex: "<<gindex<<std::endl;
			}
			indices[closest_ind] = mean[closest_ind]+sqrt(var[closest_ind])*gindex;
			std::cout<<"  Point #"<<closest_ind<<" New Index "<<indices[closest_ind]<<std::endl;
		}
	}
}

int SearchRelay::Decision(){
	int target=0;
	//Find largest index

	for(int i=0;i<indices.size();i++){
		std::cout<<"Point: "<<i<<" Index: "<<indices[i]<<std::endl;
		if(indices[i]>indices[target]){
			target = i;
		}
	}

	std::cout<<"Decided on Point #"<<target<<" with Index: "<<indices[target]<<std::endl;
	return target;
}

void SearchRelay::GetWaypoints(){ //Waypoints Ordered
	std::string filename = "relay_waypoints.txt";
	std::string one_point;
	std::ifstream waypointsfile("relay_waypoints.txt",std::ifstream::in);
	int counter=0;

	while(waypointsfile.good()){
		std::cout<<"ReadingPoints"<<std::endl;
		getline(waypointsfile,one_point);
		int pos = one_point.find(',');

		if(pos>0){
			std::cout<<one_point<<std::endl;
			std::cout<<counter<<std::endl;
			std::string subx = one_point.substr(0,pos-1);
			wpx.push_back(atof(subx.c_str()));
			std::cout<<subx<<std::endl;
			std::string suby = one_point.substr(pos+1);
			std::cout<<suby<<std::endl;
			wpy.push_back(atof(suby.c_str()));
			seglist.add_vertex(atof(subx.c_str()),atof(suby.c_str()));
			mean.push_back(0.0);
			var.push_back(0.0);
			std::vector<double> temp_vec;
			data[counter] = temp_vec;
			indices.push_back(0.0);
			counter++;
		}
	}
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
