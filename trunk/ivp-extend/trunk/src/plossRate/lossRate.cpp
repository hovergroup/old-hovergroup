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
    	  lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info(msg.GetString());
      }
      else if(key=="ACOMMS_TRANSMIT_SIMPLE"){
    	  lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info(msg.GetString());
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
	m_Comms.Register("ACOMMS_TRANSMIT_SIMPLE",0);

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool lossRate::Iterate()
{
   // happens AppTick times per second
	if(MOOSTime()-last_time>timeout)
	{

	}

   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool lossRate::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}
