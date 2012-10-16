/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Tulip26bit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "Tulip26bit.h"
//---------------------------------------------------------
// Constructor

Tulip26bit::Tulip26bit() {
}

//---------------------------------------------------------
// Destructor

Tulip26bit::~Tulip26bit() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Tulip26bit::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Tulip26bit::OnConnectToServer() {
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	double receive_period, receive_offset;
	m_MissionReader.GetConfigurationParam("receive_period", receive_period);
	m_MissionReader.GetConfigurationParam("receive_offset", receive_offset);
	m_AcommsTimer.setReceiveTiming( receive_period, receive_offset );

	double transmit_period, transmit_offset;
	m_MissionReader.GetConfigurationParam("transmit_period", transmit_period);
	m_MissionReader.GetConfigurationParam("transmit_offset", transmit_offset);
	m_AcommsTimer.setTransmitTiming( transmit_period, transmit_offset );

	double receive_extension;
	m_MissionReader.GetConfigurationParam("receive_extension", receive_extension);
	m_AcommsTimer.setReceivingExtension( receive_extension );

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Tulip26bit::Iterate() {
	// happens AppTick times per second
	m_AcommsTimer.doWork();

	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Tulip26bit::OnStartUp() {
	// happens before connection is open

	return (true);
}

