/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Eric.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "Eric.h"

//---------------------------------------------------------
// Constructor

Eric::Eric()
{
	transmit = false;
	wait_time = pt::seconds(5);
	last = pt::ptime(pt::pos_infin);
	mlast = 0;

	x.push_back(26.4);
	x.push_back(157.1184);

	y.push_back(-20);
	y.push_back(-226.5041);

}

//---------------------------------------------------------
// Destructor

Eric::~Eric()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Eric::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();

		if(key=="ACOMMS_DRIVER_STATUS"){
			if(msg.GetString()=="ready"){
				transmit = true;
			}
			else{
				transmit = false;
			}
		}

		else if(key=="HEADING_DATA"){
			heading = msg.GetDouble();
		}

		else if(key=="X_DATA"){
					navx = msg.GetDouble();
				}

		else if(key=="Y_DATA"){
					navy = msg.GetDouble();
				}

		else if(key=="ACOMMS_RECEIVED_DATA"){
			string data = msg.GetString();
			vector<string> substrings;

			if(data.size()>0){
			int pos = 0;
				while ( data.find(",", pos) != string::npos ) {
					int newpos = data.find(",", pos);
					string temp_sub = data.substr(pos, newpos-pos);
					substrings.push_back(temp_sub);
					pos = newpos+1;
				}

			m_Comms.Notify("NAV_HEADING",substrings[0]);
			m_Comms.Notify("NAV_X",substrings[1]);
			m_Comms.Notify("NAV_Y",substrings[2]);
			}
		}

		else if(key=="MISSION_START"){
			stringstream ss;
			ss<<"points=";
			for(int i=0;i<x.size();i++){
				ss<<x[i];
				ss<<",";
				ss<<y[i];
				ss<<":";
			}
			m_Comms.Notify("MISSION_MODE","GOTO");
			m_Comms.Notify("WPT_SURVEY_UPDATES",ss.str());
		}
	}
	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer
//
bool Eric::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	 m_MissionReader.GetConfigurationParam("Role", role);

	m_Comms.Register("HEADING_DATA",0);
	m_Comms.Register("X_DATA",0);
	m_Comms.Register("Y_DATA",0);
	m_Comms.Register("MISSION_START",0);
	m_Comms.Register("GPS_PTIME",0);
	m_Comms.Register("ACOMMS_RECEIVED_DATA",0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Notify("ACOMMS_TRANSMIT_RATE",0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Eric::Iterate()
{
	if(role=="shore"){
	// happens AppTick times per second
	if(MOOSTime()-mlast>=7 && transmit){
		stringstream ss;
		ss<<heading<<","<<navx<<","<<navy;
		m_Comms.Notify("ACOMMS_TRANSMIT_DATA",ss.str());
		mlast = MOOSTime();
	}
	}
	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Eric::OnStartUp()
{
	// happens before connection is open

	return(true);
}

