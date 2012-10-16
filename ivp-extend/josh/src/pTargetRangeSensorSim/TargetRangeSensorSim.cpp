/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TargetRangeSensorSim.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "TargetRangeSensorSim.h"
//---------------------------------------------------------
// Constructor

TargetRangeSensorSim::TargetRangeSensorSim()
{
}

//---------------------------------------------------------
// Destructor

TargetRangeSensorSim::~TargetRangeSensorSim()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TargetRangeSensorSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TargetRangeSensorSim::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool TargetRangeSensorSim::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool TargetRangeSensorSim::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

