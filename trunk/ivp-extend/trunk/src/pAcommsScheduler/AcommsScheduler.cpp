/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommsScheduler.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AcommsScheduler.h"

//---------------------------------------------------------
// Constructor

AcommsScheduler::AcommsScheduler() {
	m_iterations = 0;
}

//---------------------------------------------------------
// Destructor

AcommsScheduler::~AcommsScheduler() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AcommsScheduler::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AcommsScheduler::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("period", m_period);
	m_MissionReader.GetConfigurationParam("pre_lock", m_preLock);
	m_MissionReader.GetConfigurationParam("post_lock", m_postLock);

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

bool AcommsScheduler::Iterate() {
	m_iterations++;
	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AcommsScheduler::OnStartUp() {

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void AcommsScheduler::RegisterVariables() {
	m_Comms.Register("ACOMMS_TRANSMIT_DATA", 0);
	m_Comms.Register("ACOMMS_TRANSMIT_DATA_BINARY", 0);
	m_Comms.Register("ACOMMS_TRANSMIT", 0);

	m_Comms.Register("ACOMMS_RECEIVED", 0);
	// m_Comms.Register("FOOBAR", 0);
}

