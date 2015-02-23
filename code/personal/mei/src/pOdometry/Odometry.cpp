/*
 * Mei Cheung
 *        File: Odometry.cpp
 *  Created on: 
 *      Author: 
 */

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{

	m_first_x = true;
	m_first_y = true;
	m_first_reading = true;
	m_current_x = 0;
	m_current_y = 0;
	m_previous_x = 0;
	m_previous_y = 0;
	m_total_distance = 0;

    m_iterations = 0;
    m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{

	AppCastingMOOSApp::OnNewMail(NewMail);        // Added for appcasting
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key  = msg.GetKey();

        if (key == "NAV_X") {
        	m_previous_x = m_current_x;
        	m_current_x = msg.GetDouble();
        	if (m_first_x) m_first_x=false;
        } else if (key == "NAV_Y"){
        	m_previous_y = m_current_y;
        	m_current_y = msg.GetDouble();
        	if (m_first_y) m_first_y=false;
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

bool Odometry::OnConnectToServer()
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

bool Odometry::Iterate()
{

	AppCastingMOOSApp::Iterate();                  // Added for Appcasting

	if (m_first_x||m_first_y){
		m_Comms.Notify("ODOMETRY_MSG","Waiting for both X and Y.");
	} else{
		if (m_first_reading){
			m_Comms.Notify("ODOMETRY_MSG","First reading skipped.");
			m_first_reading=false;
		}else{
			m_total_distance = m_total_distance + sqrt(pow((m_current_x-m_previous_x),2)+pow((m_current_y-m_previous_y),2));
			m_Comms.Notify("ODOMETRY_DIST",m_total_distance);
		}
	}

	AppCastingMOOSApp::PostReport();               // Added for Appcasting
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{

	AppCastingMOOSApp::OnStartUp();

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

void Odometry::RegisterVariables()
{
	AppCastingMOOSApp::RegisterVariables();

	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
}

bool Odometry::buildReport()
{
      m_msgs << "Vehicle travelled: " << m_total_distance << endl;

      return(true);
}
