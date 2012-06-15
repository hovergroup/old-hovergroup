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
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Eric::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Eric::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Eric::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

