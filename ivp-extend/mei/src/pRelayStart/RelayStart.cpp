/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayStart.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "RelayStart.h"
//---------------------------------------------------------
// Constructor

RelayStart::RelayStart()
{
}

//---------------------------------------------------------
// Destructor

RelayStart::~RelayStart()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RelayStart::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RelayStart::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RelayStart::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RelayStart::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

