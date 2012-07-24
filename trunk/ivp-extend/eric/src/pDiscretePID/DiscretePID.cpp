/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: DiscretePID.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "DiscretePID.h"
//---------------------------------------------------------
// Constructor

DiscretePID::DiscretePID()
{
}

//---------------------------------------------------------
// Destructor

DiscretePID::~DiscretePID()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DiscretePID::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DiscretePID::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	


   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool DiscretePID::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool DiscretePID::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

