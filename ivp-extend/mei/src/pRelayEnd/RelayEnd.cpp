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
	driver_ready = false;
	fudge_factor = 5; //m
	update_time = 5; //s
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
			cout << "Mail: " << receive_info.num_good_frames << "/" << receive_info.num_frames <<" frames"<< endl;

			if(receive_info.source==relay_id && receive_info.num_good_frames==receive_info.num_frames){
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA","1");
			}

			else{
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA","0");
			}
		}

		else if(key=="ACOMMS_DRIVER_STATUS"){
			if(msg.GetString()=="ready"){
				driver_ready = true;
			}
			else{
				driver_ready = false;
			}
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

	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
	m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);

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
	now = MOOSTime();
	if(now - last >= update_time){
		// happens AppTick times per second
		if(fabs(myx-end_x)<=fudge_factor && fabs(myy-end_y)<=fudge_factor){
			cout << "In Position"<<endl;
			if(driver_ready){
				cout << "Driver Ready" << endl;
				m_Comms.Notify("END_STATUS","ready");
			}
			else{
				cout << "Driver not Ready" << endl;
				m_Comms.Notify("END_STATUS","MODEM NOT READY");
			}
		}
		else{
			cout << "Not in Position" << endl;
			m_Comms.Notify("END_STATUS","NOT IN PLACE");
		}
		last = MOOSTime();
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

