/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Tulip26bit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "Tulip26bit.h"
#include "goby/acomms/connect.h"

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

	std::string vehicle_mode;
	m_MissionReader.GetConfigurationParam("vehicle_mode", vehicle_mode);

	if ( vehicle_mode == "leader" ) {

	} else if ( vehicle_mode == "follower" ) {

	} else {
		std::cout << "Invalid vehicle mode - exiting." << std::endl;
		exit(0);
	}

	goby::acomms::connect( &m_AcommsTimer.signal_debug, boost::bind( &Tulip26bit::handleDebug, this, _1) );
	goby::acomms::connect( &m_AcommsTimer.signal_updates, boost::bind( &Tulip26bit::handleDebug, this, _1) );

	return (true);
}

void Tulip26bit::handleDebug( const std::string msg ) {
	m_Comms.Notify("TULIP_DEBUG", msg);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Tulip26bit::Iterate() {
	m_AcommsTimer.doWork();

	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Tulip26bit::OnStartUp() {
	// happens before connection is open

	return (true);
}

