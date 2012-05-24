/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: lossRate.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "lossRate.h"

//---------------------------------------------------------
// Constructor

lossRate::lossRate()
{
	sent_something = false;
}

//---------------------------------------------------------
// Destructor

lossRate::~lossRate()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool lossRate::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      std::string key = msg.GetKey();

      if(key=="ACOMMS_RECEIVED_SIMPLE"){
    	 receive_info(msg.GetString());
      }
      else if(key=="ACOMMS_TRANSMIT"){
    	  transmit_info(msg.GetString());
    	  sent_something = true;
    	  last_time = MOOSTime();
      }

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool lossRate::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool lossRate::Iterate()
{
   // happens AppTick times per second
	


   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool lossRate::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}
