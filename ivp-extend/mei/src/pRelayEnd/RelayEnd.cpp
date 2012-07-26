/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayEnd.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "RelayEnd.h"
//---------------------------------------------------------
// Constructor

RelayEnd::RelayEnd()
{
	station_factor = 3; //m
	fudge_factor = 15; //m
	update_time = 7; //s
	voltage = 0;
}

//---------------------------------------------------------
// Destructor

RelayEnd::~RelayEnd()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RelayEnd::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();

		if(key=="NAV_X"){
			myx = msg.GetDouble();
		}
		else if(key=="NAV_Y"){
			myy = msg.GetDouble();
		}

		else if(key=="ACOMMS_RECEIVED_SIMPLE"){
			lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info(msg.GetString());
			if(receive_info.source==relay_id){
				cout << "Got Mail: " << receive_info.num_good_frames << "/" << receive_info.num_frames <<" frames"<< endl;
				if(receive_info.num_good_frames==receive_info.num_frames){
					m_Comms.Notify("RELAY_SUCCESS","true");
				}
				else{
					m_Comms.Notify("RELAY_SUCCESS","false");
				}
			}
		}

		else if(key=="ACOMMS_DRIVER_STATUS"){
			driver_status = msg.GetString();
			m_Comms.Notify("END_STATUS",driver_status);
		}
		else if(key=="VOLTAGE"){
			voltage = msg.GetDouble();
		}
		else if(key=="DESIRED_THRUST"){
			mythrust = msg.GetDouble();
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RelayEnd::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_MissionReader.GetConfigurationParam("RelayID",relay_id);
	m_MissionReader.GetConfigurationParam("Endx", end_x);
	m_MissionReader.GetConfigurationParam("Endy", end_y);
	m_MissionReader.GetConfigurationParam("OuterRadius",fudge_factor);
	m_MissionReader.GetConfigurationParam("InnerRadius",station_factor);

	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
	m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Register("VOLTAGE",0);
	m_Comms.Register("DESIRED_THRUST",0);

	stringstream ss;
	ss<<"points="<<end_x<<","<<end_y;
	m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
	ss.str("");
	ss<<"station_pt="<<end_x<<","<<end_y;
	m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
	m_Comms.Notify("RELAY_MODE","GOTO");
	m_Comms.Notify("MISSION_MODE","RELAY");

	m_Comms.Notify("ACOMMS_TRANSMIT_RATE",100);

	now = MOOSTime();
	last = 0;

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RelayEnd::Iterate()
{
	double offset = sqrt(pow((myx-end_x),2) + pow((myy-end_y),2));

	if(offset < station_factor){
		if(mythrust != 0){
			cout << "Turning thruster off" << endl;
			m_Comms.Notify("MOOS_MANUAL_OVERRIDE","true");
		}
	}
	else if(offset > fudge_factor){
		if(mythrust == 0){
			cout << "Turning thruster on" << endl;
			m_Comms.Notify("MOOS_MANUAL_OVERRIDE","false");
		}
	}

	now = MOOSTime();
	if(now - last >= update_time){
		cout << "Voltage: " << voltage << endl;
		cout << "Driver: " << driver_status << endl;
		cout << "Thrust: " << mythrust << endl;
		cout << "Offset: " << offset << endl;

		last = MOOSTime();
		cout << endl;
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RelayEnd::OnStartUp()
{
	// happens before connection is open

	return(true);
}
