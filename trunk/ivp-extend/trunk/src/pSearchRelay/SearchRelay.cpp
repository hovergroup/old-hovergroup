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
      //all
      if(key=="GPS_PTIME"){
    	  now = pt::time_from_string(msg.GetString());
		  if(my_role=="shore"){
			  if(last.is_pos_infinity()){last=pt::time_from_string(msg.GetString());}
		  }
      }
      //relay
      else if(key=="SEARCH_RELAY_RESET"){

            }

      else if(key=="ACOMMS_SNR_OUT"){
    	  data.push_back(msg.GetDouble());
    	  mean = gsl_stats_mean(&data[0],1,data.size());
    	  var = gsl_stats_variance(&data[0],1,data.size());
      }
      //shore
      else if(key=="SEARCH_RELAY_WAIT_TIME"){
    	  wait_time = pt::seconds((long) msg.GetDouble());
      }
      else if(key=="SEARCH_RELAY_RATE"){
    	  rate = (int) msg.GetDouble();
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

	m_Comms.Register("GPS_PTIME",0);

	if(my_role=="relay"){
		m_MissionReader.GetConfigurationParam("Mode",mode);
		m_Comms.Register("ACOMMS_SNR_OUT",0);
		m_Comms.Notify("RELAY_STATUS","ready");
		m_Comms.Register("SEARCH_RELAY_RESET",0);

		if(mode=="normal"){
			normal_indices_five = {10.141,1.1656,0.6193,0.4478,0.359,0.3035,
								   0.2645,0.2353,0.2123,0.1109,0.0761,0.0582,0.0472,0.0397,0.0343,0.0302,0.0269,0.0244};
			normal_indices_one = {39.3343,3.102,1.3428,0.9052,0.7054,0.5901,0.5123,0.4556,0.4119,
									0.223,0.1579,0.1235,0.1019,0.087,0.076,0.0675,0.0608,0.0554};
			}
		}

	else if(my_role=="end_node"){
		m_Comms.Notify("END_STATUS","ready");
	}

	else if(my_role=="shore_node"){
		m_Comms.Register("ACOMMS_TRANSMIT_SIMPLE",0);
		m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
		m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
		m_Comms.Register("SEARCH_RELAY_RATE",0);
		m_Comms.Register("RELAY_STATUS",0);
		m_Comms.Register("END_STATUS",0);
		wait_time = pt::seconds(20);
		rate = 2;
		last(pt::pos_infin);
	}

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SearchRelay::Iterate()
{
   // happens AppTick times per second

	if(my_role=="relay"){

	}

	else if(my_role=="end_node"){ //do nothing

	}

	else if(my_role=="shore_node"&&relay_status=="ready"&&end_status=="ready"){ //transmitting every wait_time seconds

		if(now-last>=wait_time){
			int length;
			std::stringstream ss;
			ss << counter;
			if(rate==0){length = 32;}
			else if(rate==2){length = 192;}
			std::string mail = ss.str()+"---"+lib_acomms_messages::getRandomString(length);
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
		}
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

std::vector<double> ComputeIndices(std::vector<double> data){
return data;
}

std::vector<double> GetWaypoints(){
	std::string filename = "relay_waypoints.txt";

}
