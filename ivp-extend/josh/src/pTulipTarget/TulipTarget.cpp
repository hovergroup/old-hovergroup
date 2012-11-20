/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TulipTarget.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "TulipTarget.h"
#include "goby/acomms/connect.h"

//---------------------------------------------------------
// Constructor

TulipTarget::TulipTarget() {
    m_WaitingForData = false;
    m_lastRangeRequestTime = 0;
}

//---------------------------------------------------------
// Destructor

TulipTarget::~TulipTarget() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TulipTarget::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        std::string key = msg.GetKey();

        if (key == "ACOMMS_DRIVER_STATUS") {
            std::string status = msg.GetString();
            if (m_AcommsStatus != "receiving" && status == "receiving")
                m_AcommsTimer.signalStartOfModemReceiving();
            m_AcommsStatus = status;

        } else if (key == "ACOMMS_RECEIVED_DATA") {
            m_ReceivedData = msg.GetString();
            m_ReceivedDataTime = msg.GetTime();
            if (m_WaitingForData
                    && abs(m_BadFramesTime - m_ReceivedDataTime) < 0.25) {
                m_AcommsTimer.signalGoodReception(m_ReceivedData);
                m_WaitingForData = false;
            }

        } else if (key == "ACOMMS_BAD_FRAMES") {
            std::string frame_status = msg.GetString();
            m_BadFramesTime = msg.GetTime();
            if (frame_status == "") {
                if (abs(m_BadFramesTime - m_ReceivedDataTime) < 0.25)
                    m_AcommsTimer.signalGoodReception(m_ReceivedData);
                else
                    m_WaitingForData = true;
            } else {
                m_AcommsTimer.signalBadReception();
            }

        } else if (key == "GPS_TIME_SECONDS") {
            m_AcommsTimer.processGpsTimeSeconds(msg.GetDouble(), msg.GetTime());
        }
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TulipTarget::OnConnectToServer() {
//    double receive_period, receive_offset;
//    m_MissionReader.GetConfigurationParam("receive_period", receive_period);
//    m_MissionReader.GetConfigurationParam("receive_offset", receive_offset);
    m_AcommsTimer.setReceiveTiming(100000, 0);

    double transmit_period, transmit_offset;
    m_MissionReader.GetConfigurationParam("transmit_period", transmit_period);
    m_MissionReader.GetConfigurationParam("transmit_offset", transmit_offset);
    m_AcommsTimer.setTransmitTiming(transmit_period, transmit_offset);

    m_MissionReader.GetValue("Community", m_name);
    MOOSToUpper(m_name);

//    double receive_extension, max_receive_error;
//    m_MissionReader.GetConfigurationParam("receive_extension",
//            receive_extension);
//    m_MissionReader.GetConfigurationParam("max_receive_error",
//    		max_receive_error);
    m_AcommsTimer.setReceivingExtension(0);
    m_AcommsTimer.setMaxReceivingError(1);

	goby::acomms::connect(&m_AcommsTimer.signal_transmit,
			boost::bind(&TulipTarget::onTransmit, this));

    goby::acomms::connect(&m_AcommsTimer.signal_debug,
            boost::bind(&TulipTarget::handleDebug, this, _1));
    goby::acomms::connect(&m_AcommsTimer.signal_updates,
            boost::bind(&TulipTarget::handleUpdate, this, _1));

    m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);
    m_Comms.Register("ACOMMS_DRIVER_STATUS", 0);
    m_Comms.Register("ACOMMS_BAD_FRAMES", 0);
    m_Comms.Register("GPS_TIME_SECONDS", 0);

    return (true);
}

void TulipTarget::handleDebug(const std::string msg) {
    m_Comms.Notify("TULIP_DEBUG", msg);
}

void TulipTarget::handleUpdate(const std::string msg) {
    m_Comms.Notify("TULIP_UPDATES", msg);
}

void TulipTarget::onTransmit() {
	std::string data = "T";
	m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", data.data(), 1);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool TulipTarget::Iterate() {
    m_AcommsTimer.doWork(MOOSTime());

    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool TulipTarget::OnStartUp() {
    // happens before connection is open

    return (true);
}

