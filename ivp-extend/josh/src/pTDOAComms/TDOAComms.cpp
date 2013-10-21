/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOAComms.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "TDOAComms.h"
#include "HoverAcomms.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TDOAComms::TDOAComms() {
	resetOutput();
}

//---------------------------------------------------------
// Destructor

TDOAComms::~TDOAComms() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TDOAComms::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();

		// handle incoming acomms events
		if (key=="ACOMMS_RECEIVED") {
			HoverAcomms::AcommsReception reception;
			if (reception.parseFromString(msg.GetString())) {
//				if (reception.getSource()==m_sourceID)
//					m_dataComplete = true;
			} else {
			    std::cout << "parse error" << std::endl;
			}
		}

		// set offset of target according to acomms scheduler
		else if (key == "ACOMMS_SCHEDULER_OFFSET") {
			m_targetOffset = msg.GetDouble();
		}

	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TDOAComms::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("ID", m_id);

	m_outgoingUpdate.set_local_id(m_id);
	m_outgoingUpdate.set_cycle_state(TDOAUpdate_StateEnum_PAUSED);

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOAComms::Iterate() {
	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TDOAComms::OnStartUp() {
	return (true);
}

void TDOAComms::resetOutput() {
	m_outgoingUpdate.mutable_data()->Clear();
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void TDOAComms::RegisterVariables() {
	m_Comms.Register("ACOMMS_RECEIVED", 0);
	m_Comms.Register("NAV_X", 0);
	m_Comms.Register("NAV_Y", 0);
	m_Comms.Register("ACOMMS_SCHEDULER_OFFSET", 0);
}

