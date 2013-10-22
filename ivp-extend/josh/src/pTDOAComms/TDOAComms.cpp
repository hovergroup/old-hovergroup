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
	m_offsets = std::vector<double>(3, 0);

	m_paused = true;
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
			m_slotFunctions.base_offset = m_targetOffset;
		}

		// adjust offset or period after launch
		else if (key == "TDOA_OFFSET") {
			m_localOffset = msg.GetDouble();
		} else if (key == "TDOA_PERIOD") {
			m_slotFunctions.period = msg.GetDouble();
		}

		// pause/unpause
		else if (key == "TDOA_COMMAND") {
			string sval = MOOSToUpper(msg.GetString());
			if (sval == "RUN") {
				m_paused = false;
			} else if (sval == "PAUSE") {
				m_paused = true;
			}
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TDOAComms::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("ID", m_id);
	m_MissionReader.GetConfigurationParam("period", m_slotFunctions.period);
	m_MissionReader.GetConfigurationParam("f1_offset", m_offsets[0]);
	m_MissionReader.GetConfigurationParam("f2_offset", m_offsets[1]);
	m_MissionReader.GetConfigurationParam("f3_offset", m_offsets[2]);
	m_MissionReader.GetConfigurationParam("target_id", m_targetID);

	m_localOffset = m_offsets[m_id];

	m_outgoingUpdate.set_local_id(m_id);
	m_outgoingUpdate.set_cycle_state(TDOAUpdate_StateEnum_PAUSED);

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOAComms::Iterate() {
	// do nothing if paused
	if (m_paused)
		return true;

//	switch (m_outgoingUpdate.cycle_state()) {
//	case TDOAUpdate_StateEnum_LEADER_SLOT_COMPLETE:
//		if ()
//	}

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
	m_Comms.Register("TDOA_OFFSET", 0);
	m_Comms.Register("TDOA_PERIOD", 0);
	m_Comms.Register("TDOA_COMMAND", 0);
}

