/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayStart.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "RelayStart.h"
//---------------------------------------------------------
// Constructor

RelayStart::RelayStart()
{
	wait_time = 17; //s
	last = 0;
	relay_sync = false;
	mail_counter = 0;
}

//---------------------------------------------------------
// Destructor

RelayStart::~RelayStart()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RelayStart::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();

		//Key count: 6
		if(key=="ACOMMS_DRIVER_STATUS"){
			driver_status = msg.GetString();
		}
		else if(key=="SEARCH_RELAY_WAIT_TIME"){
			wait_time = msg.GetDouble();
			cout<<"Setting wait time: "<<wait_time<<std::endl;
		}
		else if(key=="RELAY_STATUS"){
			relay_status = msg.GetString();
		}
		else if(key=="END_STATUS"){
			end_status = msg.GetString();
		}
		else if(key=="RELAY_PAUSE"){
			pause = msg.GetString();
		}
		else if(key == "ACOMMS_RECEIVED_SIMPLE"){
			lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info(msg.GetString());
			if(receive_info.rate == 100){
				relay_sync = true;
				cout << "Relay Synced" << std::endl;
			}
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RelayStart::OnConnectToServer()
{
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_MissionReader.GetConfigurationParam("Rate",rate);
	if(rate==0){length = 32;}
	else if(rate==2){length = 192;}

	//Reg Count: 6
	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
	m_Comms.Register("RELAY_STATUS",0);
	m_Comms.Register("END_STATUS",0);
	m_Comms.Register("RELAY_PAUSE",0);
	m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);

	m_Comms.Notify("ACOMMS_TRANSMIT_RATE",rate);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RelayStart::Iterate()
{
	// happens AppTick times per second

	if(pause=="false"){
		if(relay_status=="ready"){
			if(end_status=="ready"){
				if(driver_status = "ready"){

					//transmit as soon as possible
					double time_since = MOOSTime()-last;
					cout << time_since << endl;

					if( (time_since>=wait_time) || (relay_sync)){

						relay_sync = false;

						stringstream ss;
						ss << mail_counter;

						string mail = ss.str()+"---"+getRandomString(length);
						m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
						last = MOOSTime();
						mail_counter++;

					} else{cout<<"MMDRIVER: " << driver_status <<endl;}
				} else{cout << end_status << endl;}
			} else{cout << relay_status << endl;}
		} else{cout << "EXPERIMENT PAUSED" << endl;}

		return(true);
	}

	//---------------------------------------------------------
	// Procedure: OnStartUp()

	bool RelayStart::OnStartUp()
	{
		// happens before connection is open

		return(true);
	}

