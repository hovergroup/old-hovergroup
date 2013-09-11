/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ProtoReporter.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ProtoReporter.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ProtoReporter::ProtoReporter() {
}

//---------------------------------------------------------
// Destructor

ProtoReporter::~ProtoReporter() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ProtoReporter::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		if (key == "ACOMMS_DRIVER_STATUS") {
			nr.set_acommsdriverrunning(true);
		} else if (key == "NAV_X") {
			nr.set_nav_x(msg.GetDouble());
		} else if (key == "NAV_Y") {
			nr.set_nav_y(msg.GetDouble());
		} else if (key == "NAV_HEADING") {
			nr.set_heading(msg.GetDouble());
		} else if (key == "NAV_SPEED") {
			nr.set_speed(msg.GetDouble());
		} else if (key == "NAV_DEPTH") {
			nr.set_depth(msg.GetDouble());
		} else if (key == "VOLTAGE") {
			nr.set_voltage(msg.GetDouble());
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ProtoReporter::OnConnectToServer() {
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);
	m_MissionReader.GetValue("Community", m_name);
	nr.set_name(m_name);

	std::string platform;
	m_MissionReader.GetConfigurationParam("PLATFORM_TYPE", platform);
	platform = MOOSToUpper(platform.c_str());
	if (platform == "KAYAK") {
		nr.set_platformtype(ProtoNodeReport_TypeEnum_KAYAK);
	} else if (platform == "GLIDER") {
		nr.set_platformtype(ProtoNodeReport_TypeEnum_GLIDER);
	} else if (platform == "AUV") {
		nr.set_platformtype(ProtoNodeReport_TypeEnum_AUV);
	}

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ProtoReporter::Iterate() {
	if (MOOSTime() - m_lastAcommsStatusUpdate > 6) {
		nr.set_acommsdriverrunning(false);
	}

	nr.set_time(MOOSTime());

	std::string out = nr.SerializeAsString();
	if (!out.empty())
		m_Comms.Notify("PROTO_REPORT_LOCAL", (void*) out.data(), out.size());

	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool ProtoReporter::OnStartUp() {
	return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void ProtoReporter::RegisterVariables() {
	m_vars.push_back("NAV_X");
	m_vars.push_back("NAV_Y");
	m_vars.push_back("NAV_HEADING");
	m_vars.push_back("NAV_SPEED");
	m_vars.push_back("NAV_DEPTH");
	m_vars.push_back("VOLTAGE");
	m_vars.push_back("ACOMMS_DRIVER_STATUS");

	for (int i=0; i<m_vars.size(); i++) {
		m_Comms.Register(m_vars[i], 0);
	}
}

