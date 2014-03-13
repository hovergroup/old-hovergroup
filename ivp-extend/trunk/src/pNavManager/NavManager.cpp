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
	source = none;

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
		else if (key == "GPS_X") {
			if (source == gps)
				m_Comms.Notify("NAV_X", msg.GetDouble());
			else
				alt_x = msg.GetDouble();
			gps_update_time = MOOSTime();
		} else if (key == "GPS_Y") {
			if (source == gps)
				m_Comms.Notify("NAV_Y", msg.GetDouble());
			else
				alt_y = msg.GetDouble();
		} else if (key == "GPS_SPEED") {
			if (source == gps)
				m_Comms.Notify("NAV_SPEED", msg.GetDouble());
		}

		// if using rtk, mirror rtk
		else if (key == "RTK_X") {
			if (source == rtk)
				m_Comms.Notify("NAV_X", msg.GetDouble());
			else
				alt_x = msg.GetDouble();
			rtk_update_time = MOOSTime();
		} else if (key == "RTK_Y") {
			if (source == rtk)
				m_Comms.Notify("NAV_Y", msg.GetDouble());
			else
				alt_y = msg.GetDouble();
            rtk_update_time = MOOSTime();
		} else if (key == "RTK_SPEED") {
			if (source == rtk)
				m_Comms.Notify("NAV_SPEED", msg.GetDouble());
            rtk_update_time = MOOSTime();
		}

		else if (key == "RTK_QUALITY" && rtk) {
			switch ((int) msg.GetDouble()) {
			case 1:
				rtk_status = FIX;
				break;
			case 2:
				rtk_status = FLOAT;
				break;
			case 5:
				rtk_status = SINGLE;
				break;
			default:
				rtk_status = NONE;
				break;
			}
            rtk_update_time = MOOSTime();
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool NavManager::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("timeout", TIMEOUT);
	m_MissionReader.GetValue("Community", my_name);

	bool ok1, ok2, ok3, ok4;
	vector<string> sources (4, "");
	ok1 = m_MissionReader.GetConfigurationParam("source1", sources[0]);
	ok2 = m_MissionReader.GetConfigurationParam("source2", sources[1]);
	ok3 = m_MissionReader.GetConfigurationParam("source3", sources[2]);
	ok4 = m_MissionReader.GetConfigurationParam("source4", sources[3]);

	if (!ok1 || !ok2 || !ok3 || !ok4) {
		cout << "Missing source preference." << endl;
		exit (1);
	}

	for (int i=0; i<sources.size(); i++) {
		MOOSToUpper(sources[i]);
		if (sources[i] == "RTK_FIX")
			source_priorities.push_back(rtk_fix);
		else if (sources[i] == "RTK_FLOAT")
			source_priorities.push_back(rtk_float);
		else if (sources[i] == "RTK_SINGLE")
			source_priorities.push_back(rtk_single);
		else if (sources[i] == "GPS")
			source_priorities.push_back(gps_internal);
		else {
			cout << "Invalid source preference." << endl;
			exit (1);
		}
	}

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
    }

	// post alternate point
	if (source == rtk && MOOSTime()-last_point_post_time > 1) {
		last_point_post_time == MOOSTime();
		XYPoint p(alt_x, alt_y);
		p.set_label(my_name + "_alt");
		p.set_vertex_size(3);
		m_Comms.Notify("VIEW_POINT", p.get_spec());
	}

	int i = 0;
	bool decided = false;
	while (i < source_priorities.size() && !decided) {
		switch (source_priorities[i]) {
		case gps_internal:
			if (gps_available && gps_lock) {
				setSource(gps);
				decided = true;
			}
			break;

		case rtk_fix:
			if (rtk_available && rtk_status==FIX) {
				setSource(rtk);
				decided = true;
			}
			break;

		case rtk_float:
			if (rtk_available && rtk_status==FLOAT) {
				setSource(rtk);
				decided = true;
			}
			break;

		case rtk_single:
			if (rtk_available && rtk_status==SINGLE) {
				setSource(rtk);
				decided = true;
			}
			break;
		}
		i++;
	}
	if (!decided)
		setSource(none);

	return (true);
}

void NavManager::postSource() {
    last_source_post_time = MOOSTime();
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
	if (new_val != source) {
		source = new_val;
		postSource();
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
	m_Comms.Register("RTK_QUALITY", 0);
}

