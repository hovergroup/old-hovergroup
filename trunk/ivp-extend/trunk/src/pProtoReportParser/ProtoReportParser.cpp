/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ProtoReportParser.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ProtoReportParser.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ProtoReportParser::ProtoReportParser() {
}

//---------------------------------------------------------
// Destructor

ProtoReportParser::~ProtoReportParser() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ProtoReportParser::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		if (key == "PROTO_REPORT") {
			ProtoNodeReport pnr;
			if (pnr.ParseFromString(msg.GetString())) {
				NodeRecord nr;
				nr.setX(pnr.x());
				nr.setY(pnr.y());
				nr.setHeading(pnr.heading());
				nr.setSpeed(pnr.speed());
				if (pnr.has_depth())
					nr.setDepth(pnr.depth());
				nr.setName(pnr.vehicle_name());
				nr.setTimeStamp(pnr.time_stamp());

				switch (pnr.platform_type()) {
				case ProtoNodeReport_PlatformTypeEnum_KAYAK:
					nr.setType("KAYAK");
					break;
				case ProtoNodeReport_PlatformTypeEnum_AUV:
					nr.setType("AUV");
					break;
				case ProtoNodeReport_PlatformTypeEnum_GLIDER:
					nr.setType("GLIDER");
					break;
				default:
					nr.setType("unknown");
					break;
				}

				m_Comms.Notify("NODE_REPORT", nr.getSpec());
			}
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ProtoReportParser::OnConnectToServer() {
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ProtoReportParser::Iterate() {
	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool ProtoReportParser::OnStartUp() {
	return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void ProtoReportParser::RegisterVariables() {
	m_Comms.Register("PROTO_REPORT");
	// m_Comms.Register("FOOBAR", 0);
}

