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

    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{
    m_iterations++;
    return(true);
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
}
