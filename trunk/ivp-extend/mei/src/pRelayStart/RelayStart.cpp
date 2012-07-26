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

		if(key=="ACOMMS_DRIVER_STATUS"){
			driver_status = msg.GetString();
		}

		else if(key=="START_TRANSMIT_NOW"){
			if(msg.GetString()=="reset"){}
			else{
				if(driver_status != "ready"){
					cout << "Driver: " << driver_status << endl;
					m_Comms.Notify("START_TRANSMITTED","false");
				}
				else{
					stringstream ss;
					ss << mail_counter << "---";
					ss << getRandomString(length);
					ss.flush();
					cout << "Transmitting: "<< ss.str() << endl;
					m_Comms.Notify("ACOMMS_TRANSMIT_DATA",ss.str());
					m_Comms.Notify("START_TRANSMITTED","true");
					mail_counter++;
				}
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

	m_Comms.Notify("START_TRANSMIT_NOW","reset");

	m_MissionReader.GetConfigurationParam("Rate",rate);

	if(rate==0){length = 32;}
	else if(rate==2){length = 192;}

	m_Comms.Notify("ACOMMS_TRANSMIT_RATE",rate);

	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Register("START_TRANSMIT_NOW",0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RelayStart::Iterate()
{
	// happens AppTick times per second

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
