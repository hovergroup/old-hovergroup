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
	m_WaitingForData = false;
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
		std::string key = msg.GetKey();

		if ( key == "ACOMMS_DRIVER_STATUS") {
			std::string status = msg.GetString();
			if ( m_AcommsStatus != "receiving" && status == "receiving" )
				m_AcommsTimer.signalStartOfModemReceiving();
			m_AcommsStatus = status;

		} else if ( key == "ACOMMS_RECEIVED_DATA" ) {
			m_ReceivedData = msg.GetString();
			m_ReceivedDataTime = msg.GetTime();
			if ( m_WaitingForData && abs(m_BadFramesTime-m_ReceivedDataTime) < 0.25 ) {
				m_AcommsTimer.signalGoodReception( m_ReceivedData );
				m_WaitingForData = false;
			}

		} else if ( key == "ACOMMS_BAD_FRAMES" ) {
			std::string frame_status = msg.GetString();
			m_BadFramesTime = msg.GetTime();
			if ( frame_status == "" ) {
				if ( abs( m_BadFramesTime - m_ReceivedDataTime ) < 0.25 )
					m_AcommsTimer.signalGoodReception(m_ReceivedData);
				else
					m_WaitingForData = true;
			} else {
				m_AcommsTimer.signalBadReception();
			}

		} else if ( key == "GPS_TIME_SECONDS" ) {
			m_AcommsTimer.processGpsTimeSeconds( msg.GetDouble() );

		}
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
		goby::acomms::connect( &m_AcommsTimer.signal_transmit,
				boost::bind( &Tulip26bit::onTransmit_leader, this) );
		goby::acomms::connect( &m_AcommsTimer.signal_no_receipt,
				boost::bind( &Tulip26bit::onBadReceive_leader, this) );
		goby::acomms::connect( &m_AcommsTimer.signal_receipt,
				boost::bind( &Tulip26bit::onGoodReceive_leader, this, _1) );
	} else if ( vehicle_mode == "follower" ) {
		goby::acomms::connect( &m_AcommsTimer.signal_transmit,
				boost::bind( &Tulip26bit::onTransmit_follower, this) );
		goby::acomms::connect( &m_AcommsTimer.signal_no_receipt,
				boost::bind( &Tulip26bit::onBadReceive_follower, this) );
		goby::acomms::connect( &m_AcommsTimer.signal_receipt,
				boost::bind( &Tulip26bit::onGoodReceive_follower, this, _1) );

	} else {
		std::cout << "Invalid vehicle mode - exiting." << std::endl;
		exit(0);
	}

	goby::acomms::connect( &m_AcommsTimer.signal_debug,
			boost::bind( &Tulip26bit::handleDebug, this, _1) );
	goby::acomms::connect( &m_AcommsTimer.signal_updates,
			boost::bind( &Tulip26bit::handleDebug, this, _1) );

	m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS", 0);
	m_Comms.Register("ACOMMS_BAD_FRAMES", 0);
	m_Comms.Register("GPS_TIME_SECONDS", 0);

	return (true);
}

void Tulip26bit::handleDebug( const std::string msg ) {
	m_Comms.Notify("TULIP_DEBUG", msg);
}

void Tulip26bit::handleUpdate( const std::string msg ) {
	m_Comms.Notify("TULIP_UPDATES", msg);
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

