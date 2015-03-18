/*
 * mc2922
 *        File: GenPath.cpp
 *  Created on: Mar 18, 2015
 *      Author: mc2922
 */

#include <iterator>
#include "MBUtils.h"
#include "GenPath.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool GenPath::OnConnectToServer()
{
    m_Comms.Register("VARIABLE", 0);
    return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenPath::Iterate()
{
    return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
{
    string config_val;
    m_MissionReader.GetConfigurationParam("config_var", config_val);

    return true;
}

