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
	wait_time = 20; //s
	last = 0;
	relay_sync = false;
	mail_counter = 0;

	srand((unsigned) time(NULL));

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
			cout<<"Setting wait time: "<<wait_time<<endl;
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
		else if(key == "ACOMMS_BRIDGE"){
				relay_sync = true;
				cout << "Relay Synced" << endl;
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
	m_MissionReader.GetConfigurationParam("WaitTime",wait_time);

	if(rate==0){length = 32;}
	else if(rate==2){length = 192;}

	//Reg Count: 6
	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
	m_Comms.Register("RELAY_STATUS",0);
	m_Comms.Register("END_STATUS",0);
	m_Comms.Register("RELAY_PAUSE",0);
	m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
	m_Comms.Register("ACOMMS_BRIDGE",0);

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
				if(driver_status == "ready"){

					//transmit as soon as possible
					double time_since = MOOSTime()-last;
					cout << time_since << endl;

					if( (time_since>=wait_time) || (relay_sync)){

						if(relay_sync){cout << "Relay synced" << endl;}
						relay_sync = false;

						stringstream ss;
						ss << mail_counter;

						string mail = ss.str()+"---"+getRandomString(length);
						cout << "Transmitting: "<<mail << endl;
						m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
						last = MOOSTime();
						mail_counter++;
					}
				} else{cout<<"MMDriver: " << driver_status <<endl;}
			} else{cout << "End Status: " << end_status << endl;}
		} else{cout <<"Relay Status: " << relay_status << endl;}
	} else{cout << "Experiment Paused" << endl;}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RelayStart::OnStartUp()
{
	// happens before connection is open

	return(true);
}

string RelayStart::getRandomString( int length ) {

	stringstream ss;
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
