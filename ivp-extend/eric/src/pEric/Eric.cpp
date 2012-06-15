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

	x.push_back(7.0053);
	x.push_back(23.6845);
	x.push_back(40.3638);
	x.push_back(57.0430);
	x.push_back(73.7222);
	x.push_back(90.4015);
	x.push_back(107.0807);
	x.push_back(123.7600);
	x.push_back(140.4392);
	x.push_back(157.1184);

	y.push_back(-21.0158);
	y.push_back(-43.8479);
	y.push_back(-66.6799);
	y.push_back(-89.5119);
	y.push_back(-112.3439);
	y.push_back(-135.1760);
	y.push_back(-158.0080);
	y.push_back(-180.8400);
	y.push_back(-203.6720);
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

		else if(key=="GPS_PTIME"){
			now = pt::time_from_string(msg.GetString());
		}

		else if(key=="ACOMMS_RECEIVED_DATA"){
			m_Comms.Notify("NAV_HEADING",msg.GetDouble());
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

	m_Comms.Register("HEADING_DATA",0);
	m_Comms.Register("MISSION_START",0);
	m_Comms.Register("GPS_PTIME",0);
	m_Comms.Notify("ACOMMS_TRANSMIT_RATE",0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Eric::Iterate()
{
	// happens AppTick times per second
	if(now-last>wait_time && transmit){
		stringstream ss;
		ss<<heading;
		m_Comms.Notify("ACOMMS_TRANSMIT_DATA",ss.str());
		last = pt::ptime(now);
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

