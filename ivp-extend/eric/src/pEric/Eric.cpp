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
	transmit = false;
	wait_time = pt::seconds(5);
	last = pt::ptime(pt::pos_infin);
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
      std::string key = msg.GetKey();

      if(key=="ACOMMS_DRIVER_STATUS"){
    	  if(msg.GetString()=="ready"){
    		  transmit = true;
    	  }
    	  else{
    		  transmit = false;
    	  }
      }

      else if(key=="COMPASS_HEADING_FILTERED"){
    	  heading = msg.GetDouble();
    	  transmit = true;
      }

      else if(key=="GPS_PTIME"){
      			now = pt::time_from_string(msg.GetString());
      		}

      else if(key=="ACOMMS_RECEIVED_DATA"){


    	  m_Comms.Notify("NAV_HEADING",msg.GetDouble());
      }

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer
//
bool Eric::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	m_Comms.Notify("ACOMMS_TRANSMIT_RATE",0);

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Eric::Iterate()
{
   // happens AppTick times per second
	if(now-last>wait_time && transmit){
		stringstream ss;
		ss<<heading;
		m_Comms.Notify("ACOMMS_TRANSMIT_DATA",ss.str());
		last = pt::ptime(now);
	}
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Eric::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

