/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_timer.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "acomms_timer.h"
//---------------------------------------------------------
// Constructor

acomms_timer::acomms_timer()
{
	duty_cycle = 10;
	last_time = 0;
	rate = 0;
	paused = true;
	driver_ready = false;
	counter = 0;
	data_out = "default";
}

//---------------------------------------------------------
// Destructor

acomms_timer::~acomms_timer()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool acomms_timer::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      std::string key = msg.GetKey();

      if(key == "ACOMMS_DUTY_CYCLE"){
    	  duty_cycle = msg.GetDouble();
      }
      else if(key == "ACOMMS_DRIVER_STATUS"){
    	  if(msg.GetString()=="ready"){
    		 driver_ready = true;
    	  }
    	  else{
    		  driver_ready = false;
    	  }
      }
      else if(key == "ACOMMS_TIMER_MISSION"){
    	  mode = msg.GetString();
    	  paused = true;
    	  m_Comms.Notify("ACOMMS_TIMER_PAUSED",true);
      }

      else if(key == "ACOMMS_TIMER_PAUSED"){
    	  if(msg.GetString()=="true"){
    		  paused = true;
    	  }
    	  else if(msg.GetString()=="false"){
    		  paused = false;
    	  }
      }

      else if(key=="ACOMMS_TRANSMIT_RATE"){
    	  rate = msg.GetDouble();
      }
      else if(key=="ACOMMS_COUNTER_RESET"){
    	  counter = 0;
      }
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_timer::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Register("ACOMMS_TIMER_PAUSED",0);
	m_Comms.Register("ACOMMS_TIMER_MISSION",0);
	m_Comms.Register("ACOMMS_DUTY_CYCLE",0);
	m_Comms.Register("ACOMMS_TRANSMIT_RATE",0);
	m_Comms.Register("ACOMMS_COUNTER_RESET",0);

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool acomms_timer::Iterate()
{
   // happens AppTick times per second
	double time_passed = MOOSTime()-last_time;
	//std::cout << "Time since last: " << time_passed << std::endl;
//
	if( !paused && driver_ready && (time_passed>=duty_cycle)){
		if(mode=="psktransmit"){

			if(rate!=2){m_Comms.Notify("ACOMMS_TRANSMIT_RATE",2);}
			std::stringstream ss;
			ss << counter;
			data_out = ss.str()+"---"+getRandomString(192);
			std::cout<<"Transmitting: :"<<data_out;
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
			last_time = MOOSTime();
			counter++;
		}
		else if(mode=="fsktransmit"){
			if(rate!=0){m_Comms.Notify("ACOMMS_TRANSMIT_RATE",0);}

			std::stringstream ss;
						ss << counter;
			data_out = ss.str()+"---"+getRandomString(32);
			std::cout<<"Transmitting: :"<<data_out;
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
			last_time = MOOSTime();
			counter++;
		}
		else if(mode=="switchtransmit"){
			if(rate==2){
				m_Comms.Notify("ACOMMS_TRANSMIT_RATE",0);
				std::stringstream ss;
							ss << counter;
				data_out = ss.str()+"---"+getRandomString(32);
				std::cout<<"Transmitting: :"<<data_out;
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
			}
			else if(rate==0){
				m_Comms.Notify("ACOMMS_TRANSMIT_RATE",2);
				std::stringstream ss;
							ss << counter;
				data_out = ss.str()+"---"+getRandomString(192);
				std::cout<<"Transmitting: :"<<data_out;
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
			}
			counter++;
			last_time = MOOSTime();
		}
		else if(mode=="minitransmit"){
			if(rate!=-1){m_Comms.Notify("ACOMMS_TRANSMIT_RATE",-1);}
			std::stringstream ss;
			ss << counter;
			data_out = getRandomString(2);
			std::cout<<"Transmitting: :"<<data_out;
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
			last_time = MOOSTime();
			counter++;
		}
		else{
			paused = true;
		}
	}

   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool acomms_timer::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

std::string acomms_timer::getRandomString( int length ) {
    srand((unsigned) time(NULL));

    std::stringstream ss;
    const int passLen = length;
    for (int i = 0; i < passLen; i++) {
    	char num = (char) ( rand() % 62 );
    	if ( num < 10 )
    		num += '0';
    	else if ( num < 36 )
    		num += 'A'-10;
    	else
    		num += 'a'-36;
    	ss << num;
    }

    return ss.str();
}
