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
      }

//      else if(key=="GPS_LATITUDE"){
//    	  lat = msg.GetDouble();
//      }
//      else if(key=="GPS_LONGITUDE"){
//    	  long = msg.GetDouble();
//      }
      else if(key=="COMPASS_HEADING_FILTERED"){
    	  heading = msg.GetDouble();
    	  transmit = true;
      }

      else if(key=="GPS_PTIME"){

      }

      else if(key=="ACOMMS_???"){

    	  //Recieved msg.GetString();
    	  //Decoding ->NAV_HEADING
    	  //next_time = decoded from acomms message
    	  //next_heading = decoded from acomms message

    	  m_Comms.Notify("NAV_HEADING",msg.GetDouble()); //How is the var passed?
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
	
	m_MissionReader.GetConfigurationParam("Role", role);

	//m_Comms.Register("GPS_LATITUDE", 0);
	//m_Comms.Register("GPS_LONGITUDE", 0);

	if(role=="shore"){
	m_Comms.Register("COMPASS_HEADING_FILTERED", 0);
	}
	else if(role=="kayak"){
		m_Comms.Register("ACOMMS_???",0); //What is the variable?
		m_Comms.Register("GPS_PTIME",0); //GPS TIME
	}
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Eric::Iterate()
{
   // happens AppTick times per second

	if(role=="shore"){
	if(transmit){
		//std::string mail = "COMPASS_HEADING_FILTERED";
		m_Comms.Notify("ACOMMS_TRANSMIT_RATE",-1);
		m_Comms.Notify("ACOMMS_TRANSMIT_BINARY_DATA",heading);
		transmit=false;
	}}

	else if(role=="kayak"){
		//timenow-timepassed = time duration
		//if (timeduration>nexttime)
		//{m_Comms.Notify("NAV_HEADING", forecast_heading"}
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

