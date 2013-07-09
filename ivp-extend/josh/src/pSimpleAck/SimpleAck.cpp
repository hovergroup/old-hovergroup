/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SimpleAck.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "SimpleAck.h"
//---------------------------------------------------------
// Constructor

SimpleAck::SimpleAck() {
	DELAY = 0.5;
	m_ackTime = -1;
	m_modemReady = false;
}

//---------------------------------------------------------
// Destructor

SimpleAck::~SimpleAck() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SimpleAck::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		if (key == "ACOMMS_RECEIVED") {
			if (m_reception.parseFromString(msg.GetString())) {
				m_ackTime = MOOSTime() + DELAY;
			} else {
				std::cout << "error parsing" << std::endl;
			}
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

bool SimpleAck::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("delay", DELAY);

	m_Comms.Register("ACOMMS_RECEIVED");
	m_Comms.Register("ACOMMS_DRIVER_STATUS");

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SimpleAck::Iterate() {
	if (m_ackTime!=-1 && MOOSTime()>m_ackTime && m_modemReady) {
		if (m_reception.getStatus() == HoverAcomms::GOOD)
			goodAck();
		else if (m_reception.getStatus() == HoverAcomms::BAD)
			badAck();
		else
			partialAck(m_reception.getNumBadFrames());

		m_ackTime = -1;
	}

	return (true);
}

void SimpleAck::goodAck() {
    std::vector<unsigned char> data(2, 0);
    data[0] = 0x00;
    data[1] = 0x00;

	m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", &data[0], 2);
}

void SimpleAck::badAck() {
    std::vector<unsigned char> data(2, 0);
    data[0] = 0x1f;
    data[1] = 0xff;

	m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", &data[0], 2);
}

void SimpleAck::partialAck(int badframes) {
    std::vector<unsigned char> data(2, 0);
    data[0] = 0x1f;
    data[1] = (unsigned char) badframes;

	m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", &data[0], 2);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SimpleAck::OnStartUp() {
	// happens before connection is open

	return (true);
}

