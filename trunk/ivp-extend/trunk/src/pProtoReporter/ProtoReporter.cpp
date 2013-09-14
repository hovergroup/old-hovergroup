/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ProtoReporter.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ProtoReporter.h"
#include "HelmReportUtils.h"

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
			switch ((int) msg.GetDouble()) {
			case HoverAcomms::READY:
				nr.set_acomms_status(ProtoNodeReport_AcommsStatusEnum_READY);
				break;
			case HoverAcomms::TRANSMITTING:
				nr.set_acomms_status(ProtoNodeReport_AcommsStatusEnum_TRANSMITTING);
				break;
			case HoverAcomms::RECEIVING:
				nr.set_acomms_status(ProtoNodeReport_AcommsStatusEnum_RECEIVING);
				break;
			case HoverAcomms::NOT_RUNNING:
				nr.set_acomms_status(ProtoNodeReport_AcommsStatusEnum_NOT_RUNNING);
				break;
			default:
				break;
			}
			m_lastAcommsStatusUpdate = msg.GetTime();
		} else if (key == "NAV_X") {
			nr.set_x(msg.GetDouble());
		} else if (key == "NAV_Y") {
			nr.set_y(msg.GetDouble());
		} else if (key == "NAV_HEADING") {
			nr.set_heading(msg.GetDouble());
		} else if (key == "NAV_SPEED") {
			nr.set_speed(msg.GetDouble());
		} else if (key == "NAV_DEPTH") {
			nr.set_depth(msg.GetDouble());
		} else if (key == "VOLTAGE") {
			nr.set_voltage(msg.GetDouble());
		} else if (key == "IVPHELM_STATE") {
			if (msg.GetString()=="DRIVE") {
				nr.set_helm_state(ProtoNodeReport_HelmStateEnum_DRIVE);
			} else {
				nr.set_helm_state(ProtoNodeReport_HelmStateEnum_PARK);
			}
			m_lastHelmStateUpdate = msg.GetTime();
		} else if (key == "IVPHELM_SUMMARY") {

			vector<string> svector = parseStringQ(msg.GetString(), ',');
			unsigned int i, vsize = svector.size();
			for (i = 0; i < vsize; i++) {
				string left = biteStringX(svector[i], '=');
				string right = svector[i];

				if (left == "active_bhvs") {
					nr.clear_active_behaviors();
					nr.add_active_behaviors(biteStringX(right, '$'));

					while (right.find(":")!=string::npos) {
						biteStringX(right, ':');
						nr.add_active_behaviors(biteStringX(right, '$'));
					}
				}
			}

			//"iter=45,utc_time=1379176756.94,ofnum=2,var=speed:2,var=course:163,
			//active_bhvs=goto_and_station$1379176756.94$100.00000$9$0.01000$1/1$1:
			//goto_and_return$1379176756.94$100.00000$9$0.01000$1/1$1,
			//idle_bhvs=return$0.00$n/a:Archie_Stationkeep$1379176756.94$n/a"
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
	nr.set_vehicle_name(m_name);

	std::string platform;
	m_MissionReader.GetConfigurationParam("PLATFORM_TYPE", platform);
	platform = MOOSToUpper(platform.c_str());
	if (platform == "KAYAK") {
		nr.set_platform_type(ProtoNodeReport_PlatformTypeEnum_KAYAK);
	} else if (platform == "GLIDER") {
		nr.set_platform_type(ProtoNodeReport_PlatformTypeEnum_GLIDER);
	} else if (platform == "AUV") {
		nr.set_platform_type(ProtoNodeReport_PlatformTypeEnum_AUV);
	}

	RegisterVariables();

	m_Comms.Notify("IVPHELM_REJOURNAL", "true");
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ProtoReporter::Iterate() {
	if (MOOSTime() - m_lastAcommsStatusUpdate > 6) {
		nr.set_acomms_status(ProtoNodeReport_AcommsStatusEnum_OFFLINE);
	}
	if (MOOSTime() - m_lastHelmStateUpdate > 5) {
		nr.set_helm_state(ProtoNodeReport_HelmStateEnum_MISSING);
	}

	nr.set_time_stamp(MOOSTime());

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
	m_Comms.Register("NAV_X", 0);
	m_Comms.Register("NAV_Y", 0);
	m_Comms.Register("NAV_HEADING", 0);
	m_Comms.Register("NAV_SPEED", 0);
	m_Comms.Register("NAV_DEPTH", 0);
	m_Comms.Register("VOLTAGE", 0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS", 0);
	m_Comms.Register("IVPHELM_STATE", 0);
	m_Comms.Register("IVPHELM_SUMMARY", 0);
}

