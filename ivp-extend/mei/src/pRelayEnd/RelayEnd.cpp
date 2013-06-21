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
	mythrust = 0;
	heard = "nothing";
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

		else if(key=="ACOMMS_SOURCE_ID"){
			acomms_receive_id = msg.GetDouble();
		}

		else if(key=="ACOMMS_RECEIVED_STATUS"){
			if(acomms_receive_id==relay_id){
				if(msg.GetDouble()==0){
					heard = "good";
				}
				else{
					heard = "bad";
				}
				m_Comms.Notify("RELAY_SUCCESS",heard);
			}
		}

		else if(key=="ACOMMS_DRIVER_STATUS"){
			driver_status = msg.GetString();
			m_Comms.Notify("END_STATUS",driver_status);
		}
		else if(key=="DESIRED_THRUST"){
			mythrust = msg.GetDouble();
		}
		else if(key=="RELAY_ACK"){
			relay_status = msg.GetDouble();
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
	m_Comms.Register("ACOMMS_SOURCE_ID",0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Register("DESIRED_THRUST",0);
	m_Comms.Register("RELAY_ACK",0);

	stringstream ss;
	ss<<"points="<<end_x<<","<<end_y;
	m_Comms.Notify("WPT_RELAY_UPDATES",ss.str());
	ss.str("");
	ss<<"station_pt="<<end_x<<","<<end_y;
	m_Comms.Notify("STATION_RELAY_UPDATES",ss.str());
	m_Comms.Notify("RELAY_MODE","GOTO");
	m_Comms.Notify("MISSION_MODE","RELAY");

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RelayEnd::Iterate()
{
	double offset = sqrt(pow((myx-end_x),2) + pow((myy-end_y),2));
	m_Comms.Notify("END_OFFSET",offset);
	
	if(offset < station_factor){
		if(mythrust != 0){
			m_Comms.Notify("MOOS_MANUAL_OVERRIDE","true");
			m_Comms.Notify("END_THRUST",0.0);
		}
	}
	else if(offset > fudge_factor){
		if(mythrust == 0){
			m_Comms.Notify("MOOS_MANUAL_OVERRIDE","false");
			m_Comms.Notify("END_THRUST",30.0);
		}
	}

	if(relay_status==0){

	}
	else if(relay_status==1){
		heard == "nothing";
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
