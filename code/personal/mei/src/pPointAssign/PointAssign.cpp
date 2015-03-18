/*
 * mc2922
 *        File: PointAssign.cpp
 *  Created on: Mar 18, 2015
 *      Author: mc2922
 */

#include <iterator>
#include "MBUtils.h"
#include "PointAssign.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == "VARIABLE") {
            string val = msg.GetString();
        }
    }

    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
    m_Comms.Register("VARIABLE", 0);
    return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
    return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
    string config_val;
    m_MissionReader.GetConfigurationParam("config_var", config_val);

    return true;
}

