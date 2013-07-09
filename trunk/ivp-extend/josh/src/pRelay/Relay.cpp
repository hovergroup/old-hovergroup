/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Relay.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "Relay.h"
//---------------------------------------------------------
// Constructor

Relay::Relay() {
	m_state=idle;
	m_enable = false;
	m_lastTime = -1;
	m_iteration = 0;
}

//---------------------------------------------------------
// Destructor

Relay::~Relay() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Relay::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		if (key == "RELAY_ENABLE") {
			if (msg.GetDouble()==1)
				m_enable = true;
			else
				m_enable = false;
		} else if (key == "NAV_X") {
			m_x = msg.GetDouble();
		} else if (key == "NAV_Y") {
			m_y = msg.GetDouble();
		} else if (key == "ACOMMS_DRIVER_STATUS") {
			if (msg.GetString()=="ready")
				m_modemReady = true;
			else
				m_modemReady = false;
		} else if (key == "ACOMMS_RECEIVED") {
		    if (m_reception.parseFromString(msg.GetString())) {
				if (m_reception.getSource() == SOURCE) {
					m_gotTrans = true;
					if (m_reception.getStatus() == HoverAcomms::GOOD) {
						m_transGood = true;
						m_relayData = m_reception.getData();
					} else {
						m_transGood = false;
					}
				} else if (m_reception.getSource() == DEST) {
					m_gotAck = true;
					std::string data = m_reception.getData();
					if (data.size()!=2) {
						std::cout << "ack data wrong size" << std::endl;
						m_ackGood = false;
					} else {
						if (data[1]==0x00)
							m_ackGood = true;
						else
							m_ackGood = false;
					}
				}
			} else {
				std::cout << "error parsing protobuf" << std::endl;
			}
		} else if (key == "RELAY_WAYPOINT") {
            std::string sline = msg.GetString();
            std::string sTmp = MOOSChomp(sline,",");
            m_setX = atof(sTmp.c_str());
            m_setY = atof(sline.c_str());

            std::stringstream ss;
            ss << "points=" << m_x << "," << m_y << ":" << msg.GetString();
            m_Comms.Notify("RELAY_WAYPOINT_UPDATES", ss.str());
            m_Comms.Notify("RELAY_STATION", "false");
        }
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Relay::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("relay_delay", RELAY_DELAY);
	m_MissionReader.GetConfigurationParam("repeat_delay", REPEAT_DELAY);
	m_MissionReader.GetConfigurationParam("source", SOURCE);
	m_MissionReader.GetConfigurationParam("dest", DEST);
	m_MissionReader.GetConfigurationParam("link1_timeout", LINK1_TIMEOUT);
	m_MissionReader.GetConfigurationParam("link2_timeout", LINK2_TIMEOUT);
	m_MissionReader.GetConfigurationParam("matlab_radius", MATLAB_RADIUS);

	m_Comms.Register("ACOMMS_RECEIVED");
	m_Comms.Register("ACOMMS_DRIVER_STATUS");
	m_Comms.Register("RELAY_ENABLE");
	m_Comms.Register("NAV_X");
	m_Comms.Register("NAV_Y");
	m_Comms.Register("RELAY_WAYPOINT");

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Relay::Iterate() {
	switch (m_state) {

	case idle:
		m_initX = -1;
		m_initY = -1;
		m_relayX = -1;
		m_relayY = -1;

		// start new cycle
		if (m_enable && MOOSTime()>m_lastTime+RELAY_DELAY) {
			std::cout << "Start cycle " << m_iteration << std::endl;
			initCycle();
			m_initX = m_x;
			m_initY = m_y;
			m_gotTrans = false;
			m_state = link1;
			m_lastTime = MOOSTime();
		}
		break;

	case link1:
		if (m_gotTrans) {
			if (m_transGood) {
				std::cout << "Got link 1, queuing relay." << std::endl;
				m_state = transmit;
			} else {
				std::cout << "Link 1 failed, finishing cycle." << std::endl;
				reportFailure();
				m_state=idle;
			}
			m_lastTime = MOOSTime();
			break;
		}

		// timeout
		if (MOOSTime()>m_lastTime+LINK1_TIMEOUT) {
			std::cout << "Timed out of link 1." << std::endl;
			reportFailure();
			m_state = idle;
			m_lastTime = MOOSTime();
		}
		break;

	case transmit:
		if (m_modemReady && MOOSTime()>m_lastTime+RELAY_DELAY) {
			std::cout << "Relaying" << std::endl;
			doRelay();
			m_relayX = m_x;
			m_relayY = m_y;
			m_gotAck = false;
			m_state = link2;
			m_lastTime = MOOSTime();
		}
		break;

	case link2:
		if (m_gotAck) {
			std::cout << "Got ack, finishing cycle" << std::endl;
			if (m_ackGood)
				reportSuccess();
			else
				reportFailure();
			m_state = idle;
			m_lastTime = MOOSTime();
			break;
		}

		if (MOOSTime()>m_lastTime+LINK2_TIMEOUT) {
			std::cout << "Timed out of link 2, finishing cycle." << std::endl;
			reportFailure();
			m_state = idle;
			m_lastTime = MOOSTime();
		}
		break;

	default:
		break;
	}

	return (true);
}

void Relay::initCycle() {
	m_Comms.Notify("RANDOM_TRANSMIT_INDEX", m_iteration);
	m_iteration++;
}

std::string Relay::formatResult(int iter, double x1, double y1, double x2, double y2, int success) {
	std::stringstream ss;
	ss << iter << ",";
	ss << x1 << ",";
	ss << y1 << ",";
	ss << x2 << ",";
	ss << y2 << ",";
	ss << success;
	return ss.str();
}

void Relay::reportFailure() {
	m_Comms.Notify("RELAY_RESULT", formatResult(m_iteration, m_initX, m_initY, m_relayX, m_relayY,0));
	double d1 = dist(m_initX, m_initY, m_setX, m_setY);
	double d2 = dist(m_relayX, m_relayY, m_setX, m_setY);
	if (d1 < MATLAB_RADIUS && d2 < MATLAB_RADIUS)
		m_Comms.Notify("RELAY_RESULT_MATLAB", 0.0);
}

void Relay::reportSuccess() {
	m_Comms.Notify("RELAY_RESULT", formatResult(m_iteration, m_initX, m_initY, m_relayX, m_relayY,1));
	double d1 = dist(m_initX, m_initY, m_setX, m_setY);
	double d2 = dist(m_relayX, m_relayY, m_setX, m_setY);
	if (d1 < MATLAB_RADIUS && d2 < MATLAB_RADIUS)
		m_Comms.Notify("RELAY_RESULT_MATLAB", 1.0);
}

void Relay::doRelay() {
	m_Comms.Notify("ACOMMS_TRANSMIT_DATA", m_relayData);
}

double Relay::dist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1-x2,2.0) + pow(y1-y2,2.0));
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Relay::OnStartUp() {
	// happens before connection is open

	return (true);
}

