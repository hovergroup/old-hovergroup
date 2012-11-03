/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RoundRobin.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "RoundRobin.h"
//---------------------------------------------------------
// Constructor

RoundRobin::RoundRobin()
{
}

//---------------------------------------------------------
// Destructor

RoundRobin::~RoundRobin()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RoundRobin::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RoundRobin::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RoundRobin::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RoundRobin::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

