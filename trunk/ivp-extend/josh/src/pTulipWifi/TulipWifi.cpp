/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TulipWifi.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "TulipWifi.h"
#include "goby/acomms/connect.h"

//---------------------------------------------------------
// Constructor

TulipWifi::TulipWifi() {
	m_lastUpdateTime = -1;
	m_leader = -1;
	m_follower = -1;
}

//---------------------------------------------------------
// Destructor

TulipWifi::~TulipWifi() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TulipWifi::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        std::string key = msg.GetKey();

        if (key == "LEADER_TRAVEL_TIME") {
        	m_Comms.Notify("LEADER_RANGE", msg.GetDouble()*1450.0);
        	m_leader = MOOSTime();
        } else if (key == "FOLLOWER_TRAVEL_TIME") {
        	m_Comms.Notify("FOLLOWER_RANGE", msg.GetDouble()*1450.0);
        	m_follower = MOOSTime();
        } else if (key == "FOLLOWER_WAYPOINT") {
            std::stringstream ss;
            ss << "points=" << m_followerX << "," << m_followerY << ":" << msg.GetString();
            m_Comms.Notify("TULIP_WAYPOINT_UPDATES_" + m_followerName, ss.str());
            m_Comms.Notify("TULIP_STATION_" + m_followerName, "false");
        } else if (key == "LEADER_WAYPOINT") {
            std::stringstream ss;
            ss << "points=" << m_leaderX << "," << m_leaderY << ":" << msg.GetString();
            m_Comms.Notify("TULIP_WAYPOINT_UPDATES_" + m_leaderName, ss.str());
            m_Comms.Notify("TULIP_STATION_" + m_followerName, "false");
        } else if (key == "LEADER_X") {
        	m_leaderX = msg.GetDouble();
        } else if (key == "LEADER_Y") {
        	m_leaderY = msg.GetDouble();
        } else if (key == "FOLLOWER_X") {
        	m_followerX = msg.GetDouble();
        } else if (key == "FOLLOWER_Y") {
        	m_followerY = msg.GetDouble();
        }
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TulipWifi::OnConnectToServer() {
    m_MissionReader.GetConfigurationParam("timeout", m_timeout);
    m_MissionReader.GetConfigurationParam("leader", m_leaderName);
    m_MissionReader.GetConfigurationParam("follower", m_followerName);

    m_Comms.Register("LEADER_TRAVEL_TIME", 0);
    m_Comms.Register("FOLLOWER_TRAVEL_TIME", 0);

    m_Comms.Register("LEADER_WAYPOINT", 0);
    m_Comms.Register("FOLLOWER_WAYPOINT", 0);

    m_Comms.Register("LEADER_X", 0);
    m_Comms.Register("LEADER_Y", 0);
    m_Comms.Register("FOLLOWER_X", 0);
    m_Comms.Register("FOLLOWER_Y", 0);

    return (true);
}
//---------------------------------------------------------
// Procedure: Iterate()

bool TulipWifi::Iterate() {
	// check for mismatch by age
	if (m_leader-2 > m_follower) {
		m_follower = -1;
	} else if (m_follower-2 > m_leader) {
		m_leader = -1;
	}

	// both received - push update
	if (m_leader!=-1 && m_follower!=-1) {
		m_Comms.Notify("FOLLOWER_PACKET", 1.0);
		m_lastUpdateTime = MOOSTime();
		m_leader = -1;
		m_follower = -1;
	}

	// timeout - push update
	if (MOOSTime()-m_lastUpdateTime > m_timeout) {
		if (m_leader == -1) {
			m_Comms.Notify("LEADER_RANGE", -1.0);
		} else {
			m_Comms.Notify("FOLLOWER_RANGE", -1.0);
		}
		m_Comms.Notify("FOLLOWER_PACKET", 1.0);
		m_lastUpdateTime = MOOSTime();
	}

    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool TulipWifi::OnStartUp() {
    // happens before connection is open

    return (true);
}

