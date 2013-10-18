/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: NavManager.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "NavManager.h"

using namespace std;

//---------------------------------------------------------
// Constructor

NavManager::NavManager() {
	source = rtk;

	TIMEOUT = 5;
}

//---------------------------------------------------------
// Destructor

NavManager::~NavManager() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool NavManager::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();
		if (key == "GPS_LOCK") {
			if (msg.GetDouble()==1)
				gps_lock = true;
			else
				gps_lock = false;
		}

		// if using gps, mirror gps
		if (source == gps) {
			if (key == "GPS_X") {
				m_Comms.Notify("NAV_X", msg.GetDouble());
				gps_update_time = MOOSTime();
			} else if (key == "GPS_Y") {
				m_Comms.Notify("NAV_Y", msg.GetDouble());
			} else if (key == "GPS_SPEED") {
				m_Comms.Notify("NAV_SPEED", msg.GetDouble());
			}
		}

		// if using rtk, mirror rtk
		else if (source == rtk) {
			if (key == "RTK_X") {
				m_Comms.Notify("NAV_X", msg.GetDouble());
				rtk_update_time = MOOSTime();
			} else if (key == "RTK_Y") {
				m_Comms.Notify("NAV_Y", msg.GetDouble());
			} else if (key == "RTK_SPEED") {
				m_Comms.Notify("NAV_SPEED", msg.GetDouble());
			}
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool NavManager::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("timeout", TIMEOUT);

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool NavManager::Iterate() {
	// rtk timeout - try to fallback to gps
	if (MOOSTime()-rtk_update_time > TIMEOUT && source==rtk) {
		if (gps_lock && MOOSTime()-gps_update_time < 2) {
			setSource(gps);
		} else {
			setSource(none);
		}
	}

	// gps timeout / lock lost
	if ((MOOSTime()-gps_update_time > TIMEOUT || !gps_lock) && source==gps) {
		setSource(none);
	}

	// if no nav, try rtk then gps
	if (source == none) {
		if (MOOSTime()-rtk_update_time < TIMEOUT) {
			setSource(rtk);
		} else if (MOOSTime()-gps_update_time < TIMEOUT && gps_lock) {
			setSource(gps);
		}
	}

	return (true);
}

void NavManager::setSource(NAV_SOURCE new_val) {
	source = new_val;
	switch (new_val) {
	case gps:
		m_Comms.Notify("NAV_SOURCE", "gps");
		break;
	case rtk:
		m_Comms.Notify("NAV_SOURCE", "rtk");
		break;
	case none:
		m_Comms.Notify("NAV_SOURCE", "none");
		break;
	default:
		break;
	}
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool NavManager::OnStartUp() {
	return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void NavManager::RegisterVariables() {
	m_Comms.Register("GPS_X", 0);
	m_Comms.Register("GPS_Y", 0);
	m_Comms.Register("GPS_SPEED", 0);
	m_Comms.Register("GPS_LOCK", 0);
	m_Comms.Register("RTK_X", 0);
	m_Comms.Register("RTK_Y", 0);
	m_Comms.Register("RTK_SPEED", 0);
}

