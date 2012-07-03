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
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RelayEnd::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RelayEnd::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

