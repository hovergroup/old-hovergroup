/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: KalmanFilter.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "KalmanFilter.h"
//---------------------------------------------------------
// Constructor

KalmanFilter::KalmanFilter()
{
}

//---------------------------------------------------------
// Destructor

KalmanFilter::~KalmanFilter()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool KalmanFilter::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool KalmanFilter::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool KalmanFilter::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool KalmanFilter::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

