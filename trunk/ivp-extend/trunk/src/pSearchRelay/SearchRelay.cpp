/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SearchRelay.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "SearchRelay.h"

//---------------------------------------------------------
// Constructor

SearchRelay::SearchRelay()
{
	travelling = true;
}

//---------------------------------------------------------
// Destructor

SearchRelay::~SearchRelay()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SearchRelay::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      std::string key = msg.GetKey();

      if(key=="ACOMMS_SNR_OUT"){

    	  data.push_back(msg.GetDouble());
    	  mean = gsl_stats_mean(&data[0],1,data.size());
    	  var = gsl_stats_variance(&data[0],1,data.size());
      }
      else if(key=="SEARCH_RELAY_RESET"){
    	  my_role="all_stop";
    	  m_Comms.Notify("SEARCH_RELAY_STATUS",my_role);
      }
      else if(key=="GPS_PTIME"){
    	  now = pt::time_from_string(msg.GetString());
      }
      else if(key=="SEARCH_RELAY_WAIT_TIME"){
    	  wait_time = pt::seconds((long) msg.GetDouble());
      }
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SearchRelay::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	m_MissionReader.GetConfigurationParam("Role",my_role);

	m_Comms.Register("SEARCH_RELAY_RESET",0);
	m_Comms.Register("GPS_PTIME",0);

	if(my_role=="relay"){
		m_Comms.Register("ACOMMS_SNR_OUT",0);
	}
	else if(my_role=="end_node"){
		m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
	}
	else if(my_role=="shore_node"){
		m_Comms.Register("ACOMMS_TRANSMIT_SIMPLE",0);
		m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
		m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
		wait_time = pt::seconds(20);
	}

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SearchRelay::Iterate()
{
   // happens AppTick times per second

	if(my_role=="all_stop"){

	}

	else if(my_role=="relay"){


	}

	else if(my_role=="end_node"){

	}

	else if(my_role=="shore_node"){

	}

   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SearchRelay::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}
