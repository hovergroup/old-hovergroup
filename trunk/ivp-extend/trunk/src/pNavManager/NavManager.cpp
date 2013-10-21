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

	rtk_available = false;
	gps_available = false;
	last_point_post_time = -1;
	last_source_post_time = -1;
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

		// if using gps, mirror gps, else post point
		if (key == "GPS_X") {
			if (source == gps)
				m_Comms.Notify("NAV_X", msg.GetDouble());
			gps_x = msg.GetDouble();
			gps_update_time = MOOSTime();
		} else if (key == "GPS_Y") {
			if (source == gps)
				m_Comms.Notify("NAV_Y", msg.GetDouble());
			gps_y = msg.GetDouble();
		} else if (key == "GPS_SPEED") {
			if (source == gps)
				m_Comms.Notify("NAV_SPEED", msg.GetDouble());
		}

		// if using rtk, mirror rtk
		if (key == "RTK_X") {
			if (source == rtk)
				m_Comms.Notify("NAV_X", msg.GetDouble());
			rtk_update_time = MOOSTime();
		} else if (key == "RTK_Y") {
			if (source == rtk)
				m_Comms.Notify("NAV_Y", msg.GetDouble());
		} else if (key == "RTK_SPEED") {
			if (source == rtk)
				m_Comms.Notify("NAV_SPEED", msg.GetDouble());
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool NavManager::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("timeout", TIMEOUT);
	m_MissionReader.GetValue("Community", my_name);

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool NavManager::Iterate() {
    // check rtk availability
    if (MOOSTime()-rtk_update_time < TIMEOUT) {
        rtk_available = true;
    } else {
        rtk_available = false;
    }

    // check gps availability
    if (MOOSTime()-gps_update_time < TIMEOUT && gps_lock) {
        gps_available = true;
    } else {
        gps_available = false;
    }

    // post source periodically as a heartbeat
    if (MOOSTime()-last_source_post_time > 5) {
        postSource();
        last_source_post_time = MOOSTime();
    }

	// post gps point when using rtk every second
	if (source == rtk && MOOSTime()-last_point_post_time > 1) {
		last_point_post_time == MOOSTime();
		XYPoint p(gps_x, gps_y);
		p.set_label(my_name + "_gps");
		p.set_vertex_size(3);
		m_Comms.Notify("VIEW_POINT", p.get_spec());
	}

	// rtk timeout - try to fallback to gps
	if (source == rtk && !rtk_available) {
		if (gps_lock && MOOSTime()-gps_update_time < 2) {
			setSource(gps);
		} else {
			setSource(none);
		}
	}

	// try to move from gps to rtk
	if (source == gps && rtk_available) {
	    setSource(rtk);
	}

	// gps timeout / lock lost
	if (source==gps && !gps_available) {
		setSource(none);
	}

	// if no nav, try rtk then gps
	if (source == none) {
		if (rtk_available) {
			setSource(rtk);
		} else if (gps_available) {
			setSource(gps);
		}
	}

	return (true);
}

void NavManager::postSource() {
    switch (source) {
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

void NavManager::setSource(NAV_SOURCE new_val) {
	source = new_val;
	postSource();
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

