/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver_sim.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "acomms_driver_sim.h"
//---------------------------------------------------------
// Constructor

acomms_driver_sim::acomms_driver_sim()
{
}

//---------------------------------------------------------
// Destructor

acomms_driver_sim::~acomms_driver_sim()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool acomms_driver_sim::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_driver_sim::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool acomms_driver_sim::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool acomms_driver_sim::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

