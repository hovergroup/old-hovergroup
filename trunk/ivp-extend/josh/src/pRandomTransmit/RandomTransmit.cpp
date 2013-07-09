/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RandomTransmit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "RandomTransmit.h"
//---------------------------------------------------------
// Constructor

RandomTransmit::RandomTransmit() {
	bool m_modemReady = false;
	bool m_request = false;

	int maxLength = 192;
}

//---------------------------------------------------------
// Destructor

RandomTransmit::~RandomTransmit() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RandomTransmit::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		if (key == "RANDOM_TRANSMIT_INDEX") {
			m_request = true;
			m_requestIndex = msg.GetDouble();
		} else if (key == "ACOMMS_DRIVER_STATUS") {
			if (msg.GetString()=="ready")
				m_modemReady = true;
			else
				m_modemReady = false;
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RandomTransmit::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("length", maxLength);
	
	m_Comms.Register("RANDOM_TRANSMIT_INDEX");
	m_Comms.Register("ACOMMS_DRIVER_STATUS");

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool RandomTransmit::Iterate() {
	if (m_request && m_modemReady) {
		std::stringstream ss;
		ss << m_requestIndex << "---";
		std::string sline = getRandomString(maxLength - ss.str().size());
		m_Comms.Notify("ACOMMS_TRANSMIT_DATA", ss.str() + sline);
		m_request = false;
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RandomTransmit::OnStartUp() {
	// happens before connection is open

	return (true);
}

std::string RandomTransmit::getRandomString(int length) {
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
