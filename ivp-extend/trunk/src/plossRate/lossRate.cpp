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
	timeout = 600;
	listening = false;
	clear_old = false;
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

		  all_frames[receive_info.vehicle_name] = receive_info.num_frames;
		  good_frames[receive_info.vehicle_name] = receive_info.num_good_frames;
		  bad_frames[receive_info.vehicle_name] = receive_info.num_bad_frames;
      }

      else if(key=="ACOMMS_TRANSMIT_SIMPLE"){
    	  lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info(msg.GetString());
    	  last_time = MOOSTime();

    	  if(listening){
    		  frames_sent = transmit_frames_current;
    		  transmit_frames_current = transmit_info.num_frames;
    		  clear_old = true;
    	  }
    	  else{
    		  transmit_frames_current = transmit_info.num_frames;
    		  listening = true;
    		  clear_old = false;}
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
	if(MOOSTime()-last_time>timeout||clear_old)
	{

		clear_old = false;
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
