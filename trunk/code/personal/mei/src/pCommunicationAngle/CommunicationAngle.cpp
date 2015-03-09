/*
 * Mei Cheung
 *        File: CommunicationAngle.cpp
 *  Created on: 
 *      Author: 
 */

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
	m_iterations = 0;
	m_timewarp   = 1;

	navx=0;navy=0;
	navdepth=0;navspeed=0;navheading=0;
	elev_angle=0;transmission_loss=0;
	collabxstr="SOME_UNIQUE_PLACEHOLDER";
	collabystr="SOME_UNIQUE_PLACEHOLDER";
	collabdstr="SOME_UNIQUE_PLACEHOLDER";
	collabheadingstr="SOME_UNIQUE_PLACEHOLDER";
	collabspeedstr="SOME_UNIQUE_PLACEHOLDER";

	surface_sound_speed = 1480;
	sound_speed_gradient = 0.016;
	water_depth = 6000;
	time_interval = 1;
	timer_start = clock();
	timer_end = 0;
	pathFound=false;
	collabFound=false;
}

//---------------------------------------------------------
// Destructor

CommunicationAngle::~CommunicationAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key  = msg.GetKey();

		if (key=="NAV_X"){
			navx=msg.GetDouble();
		}else if(key=="NAV_Y"){
			navy=msg.GetDouble();
		}else if(key=="NAV_DEPTH"){
			navdepth=msg.GetDouble();
		}else if(key=="NAV_HEADING"){
			navheading=msg.GetDouble();
		}else if (key=="NAV_SPEED"){
			navspeed=msg.GetDouble();
		}else if (key=="VEHICLE_NAME"){
			myname=msg.GetString();
		}else if(key=="COLLABORATOR_NAME"){
			mycollab=msg.GetString();
			collabFound = RegisterCollab();
		}else if(key==collabxstr){
			collabx=msg.GetDouble();
		}else if(key==collabystr){
			collaby=msg.GetDouble();
		}else if(key==collabdstr){
			collabdepth=msg.GetDouble();
		}else if(key==collabheadingstr){
			collabheading=msg.GetDouble();
		}else if(key==collabspeedstr){
			collabspeed=msg.GetDouble();
		}

#if 0 // Keep these around just for template
		string key   = msg.GetKey();
		string comm  = msg.GetCommunity();
		double dval  = msg.GetDouble();
		string sval  = msg.GetString();
		string msrc  = msg.GetSource();
		double mtime = msg.GetTime();
		bool   mdbl  = msg.IsDouble();
		bool   mstr  = msg.IsString();
#endif
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	m_MissionReader.GetConfigurationParam("surface_sound_speed", surface_sound_speed);
	m_MissionReader.GetConfigurationParam("sound_speed_gradient", sound_speed_gradient);
	m_MissionReader.GetConfigurationParam("water_depth", water_depth);
	m_MissionReader.GetConfigurationParam("time_interval", time_interval);

	RegisterVariables();
	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{
	if(collabFound){	//Wait for Collaborator First
		timer_end = clock(); //Current time
		if(abs(timer_start-timer_end)>time_interval){	//Check timing

			//Calculate Acoustic Path

			timer_start = clock(); //Restart timer
		}
	}
	m_iterations++;
	return(true);
}

void CommunicationAngle::PublishAnswer()
{
	stringstream msgOut;
	if(pathFound){
		msgOut<<"elev_angle="<<elev_angle<<",";
		msgOut<<"transmission_angle="<<transmission_loss<<",";
	}else{
		msgOut<<"elev_angle=NaN,";
		msgOut<<"transmission_angle=NaN,";
	}
	msgOut<<"id=mc2922@mit.edu";
	m_Comms.Notify("ACOUSTIC_PATH",msgOut.str());
	msgOut.str("");
	msgOut<<"x="<<goto_x<<",";
	msgOut<<"y="<<goto_y<<",";
	msgOut<<"depth="<<goto_d<<",";
	msgOut<<"id=mc2922@mit.edu";
}

bool CommunicationAngle::RegisterCollab(){
	stringstream friendVars;
	friendVars<<mycollab<<"_NAV_X";
	collabxstr = friendVars.str();
	m_Comms.Register(friendVars.str(),0);

	friendVars.str("");
	friendVars<<mycollab<<"_NAV_Y";
	collabystr = friendVars.str();
	m_Comms.Register(friendVars.str(),0);

	friendVars.str("");
	friendVars<<mycollab<<"_NAV_DEPTH";
	collabdstr = friendVars.str();
	m_Comms.Register(friendVars.str(),0);

	friendVars.str("");
	friendVars<<mycollab<<"_NAV_HEADING";
	collabheadingstr = friendVars.str();
	m_Comms.Register(friendVars.str(),0);

	friendVars.str("");
	friendVars<<mycollab<<"_NAV_SPEED";
	collabspeedstr = friendVars.str();
	m_Comms.Register(friendVars.str(),0);
	return true;
}
//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle::OnStartUp()
{
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
		list<string>::iterator p;
		for(p=sParams.begin(); p!=sParams.end(); p++) {
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);

			if(param == "FOO") {
				//handled
			} else if(param == "BAR") {
				//handled
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
	return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
	m_Comms.Register("VEHICLE_NAME",0);
	m_Comms.Register("COLLABORATOR_NAME",0);
	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
	m_Comms.Register("NAV_DEPTH",0);
	m_Comms.Register("NAV_HEADING",0);
	m_Comms.Register("NAV_SPEED",0);
	m_Comms.Register("ACOUSTIC_PATH_REF",0);
	m_Comms.Register("CONNECTIVITY_LOCATION_REF",0);
}
