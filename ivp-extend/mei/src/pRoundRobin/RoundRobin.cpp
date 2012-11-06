/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RoundRobin.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "RoundRobin.h"
//---------------------------------------------------------
// Constructor

RoundRobin::RoundRobin()
{
	connected=1;
	transmissions=0;
	action="paused";
	current_point=0;
}

//---------------------------------------------------------
// Destructor

RoundRobin::~RoundRobin()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RoundRobin::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();

		if(key=="RR_PAUSE"){
			if(msg.GetString() != "reset"){
				if(msg.GetString() == "false"){
					action = "start_transmit_now";
				}
				else{
					action = "paused";
				}
			}
		}
		else if(key=="ACOMMS_RECEIVED_DATA"){
			if(msg.GetString() != "reset"){
				if(action == "ticking"){
					mail = msg.GetString();

					if(mail.size()!=length){
						handleDebug("Bad Receive from Start");
						transmissions++;
						action = "start_transmit_now";
					}
					else{
						action = "relay_wait";
						start_time = MOOSTime();
					}
				}
				else{
					handleDebug("Heard Erroneous Message");
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
		else if(key=="RR_ACTION"){
			if(msg.GetString()=="redraw"){
				m_Comms.Notify("VIEW_SEGLIST",waypoints_msg);
			}
			else if(msg.GetString()=="tellme"){
				stringstream ss;
				ss << transmissions;
				handleDebug(ss.str());
			}
		}
		else if(key=="RELAY_MODE"){
			relay_mode = msg.GetString();
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RoundRobin::OnConnectToServer()
{
	m_MissionReader.GetConfigurationParam("ThroughDelay", through_transmission_delay);
	m_MissionReader.GetConfigurationParam("RelayDelay", relay_transmission_delay);
	m_MissionReader.GetConfigurationParam("WaitTime", wait_time);
	m_MissionReader.GetConfigurationParam("Rate", rate);
	m_MissionReader.GetConfigurationParam("Transmissions",transmissions_per_segment);

	setLength(rate);

	m_Comms.Notify("RR_PAUSE","reset");
	m_Comms.Notify("START_TRANSMITTED","reset");
	m_Comms.Notify("RR_ACTION","reset");

	m_Comms.Register("ACOMMS_RECEIVED_DATA",0);
	m_Comms.Register("START_TRANSMITTED",0);
	m_Comms.Register("RR_PAUSE",0);
	m_Comms.Register("RR_ACTION",0);
	m_Comms.Register("RELAY_MODE",0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RoundRobin::Iterate(){
	//Initialize avoiding reconnection reset
	if(connected==1){
		GetWaypoints();
		RRGoto(wpx[0],wpy[0]);
		connected++;
	}

	//Position State
	if(transmissions==0){
		if(relay_mode=="KEEP"){
			action = "start_transmit_now";
		}
	}
	else if((int)transmissions % transmissions_per_segment == 0){
	    if(relay_mode=="KEEP"){
            if(current_point<total_points){
                current_point++;
            }
            else{
                current_point=0;
            }
            RRGoto(wpx[current_point],wpy[current_point]);
        }
	}

	//Acoustic State
	if(action == "start_transmit_now"){
		m_Comms.Notify("START_TRANSMIT_NOW","true");
		start_time = MOOSTime();
		action = "waiting";
	}

	else if(action == "waiting"){
		double time_elapsed = MOOSTime() - start_time;
		if(time_elapsed > wait_time){
			handleDebug("No Wifi Link with Start - ReTrying");
			action = "start_transmit_now";
		}
	}

	else if(action == "sync_with_start"){
		start_time = MOOSTime();
		action = "ticking";
	}

	else if(action == "ticking"){
		double time_elapsed = MOOSTime() - start_time;
		if(time_elapsed > wait_time){
			handleDebug("No Sync with Start");
			action = "start_transmit_now";
			transmissions++;
		}
	}

	else if(action == "relay_wait"){
		handleDebug("Through Transmission Wait");
		double time_elapsed = MOOSTime() - start_time;
		if(time_elapsed > through_transmission_delay){
			action = "relay";
		}
	}

	else if(action == "relay"){
		m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
		action = "start_transmit_wait";
		start_time = MOOSTime();
		transmissions++;
	}

	else if(action == "start_transmit_wait"){
		handleDebug("Relay Transmission Wait");
		double time_elapsed = MOOSTime() - start_time;
		if(time_elapsed > relay_transmission_delay){
			action = "start_transmit_now";
		}
	}

return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RoundRobin::OnStartUp()
{
	// happens before connection is open

	return(true);
}

void RoundRobin::GetWaypoints(){
	string filename = "relay_waypoints.txt";
	string one_point;
	ifstream waypointsfile("relay_waypoints.txt",std::ifstream::in);
	total_points = 0;

	handleDebug("Reading Points");

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
			waypoints_msg=ss.str();
			m_Comms.Notify("VIEW_MARKER",waypoints_msg);
		}
	}

	waypoints_msg = seglist.get_spec("waypoints");
	stringstream ss;
	ss<<"Read "<<total_points<<" points";
	handleDebug(ss.str());
}

void RoundRobin::handleDebug(string debug_msg){
	m_Comms.Notify("ROUND_ROBIN_MESSAGE",debug_msg);
}

void RoundRobin::RRGoto(double x, double y){
	stringstream ss;
	ss<<"points="<<x<<","<<y;
	m_Comms.Notify("WPT_RR_UPDATES",ss.str());

	ss.str("");
	ss<<"station_pt="<<x<<","<<y;
	m_Comms.Notify("STATION_RR_UPDATES",ss.str());

	m_Comms.Notify("MISSION_MODE","ROUNDROBIN");
	m_Comms.Notify("RELAY_MODE","GOTO");
}

void RoundRobin::setLength(int rate_in){
	switch(rate_in){
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
}
