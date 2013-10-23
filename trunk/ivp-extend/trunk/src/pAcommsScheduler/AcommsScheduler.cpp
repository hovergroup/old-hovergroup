/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommsScheduler.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AcommsScheduler.h"
#include "JoshUtils.h"

//---------------------------------------------------------
// Constructor

AcommsScheduler::AcommsScheduler() {
	m_lockEnabled = false;
	m_state = pre_start;
	m_haveTransmission = false;
    m_dataComplete = false;
    m_receiveComplete = false;
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
		if (key=="ACOMMS_DRIVER_STATUS") {
			HoverAcomms::DriverStatus val = static_cast<HoverAcomms::DriverStatus>(msg.GetDouble());
			if (val==HoverAcomms::RECEIVING) {
				// start of new receipt
				m_lastStart = JoshUtil::getSystemTimeSeconds();
				// reset
				m_dataComplete = false;
				m_receiveComplete = false;
			} else if (m_driverStatus==HoverAcomms::RECEIVING && val==HoverAcomms::READY) {
				// end of receipt
				m_lastEnd = JoshUtil::getSystemTimeSeconds();
				// increment
				m_receiveComplete = true;
			}
			m_driverStatus = val;
		} else if (key=="ACOMMS_RECEIVED") {
			HoverAcomms::AcommsReception reception;
			if (reception.parseFromString(msg.GetString())) {
				if (reception.getSource()==m_sourceID)
					m_dataComplete = true;
			} else {
			    std::cout << "parse error" << std::endl;
			}
		} else if (key=="ACOMMS_TRANSMIT_DATA_BINARY") {
			m_haveTransmission = true;
			m_transmissionData = msg.GetString();
			m_transmissionSource = ACOMMS_TRANSMIT_DATA_BINARY;
		} else if (key=="ACOMMS_TRANSMIT_DATA") {
			m_haveTransmission = true;
			m_transmissionData = msg.GetString();
			m_transmissionSource = ACOMMS_TRANSMIT_DATA;
		} else if (key=="ACOMMS_TRANSMIT") {
			m_haveTransmission = true;
            m_transmissionData = msg.GetString();
			m_transmissionSource = ACOMMS_TRANSMIT;
		}
	}

	if (m_dataComplete && m_receiveComplete) { // got both status timings and data output
		m_observations.push_back(std::pair<double,double>(m_lastStart,m_lastEnd));
		while (m_observations.size()>MAX_OBSERVATIONS) {
			m_observations.pop_front();
		}
		double sum1=0, sum2=0;
		for (int i=0; i<m_observations.size(); i++) {
			double slot;
			sum1 += modf(m_observations[i].first/m_period, &slot)*m_period;
			sum2 += m_observations[i].second - m_observations[i].first;
		}
		m_meanOffset = sum1/m_observations.size();
		m_meanDuration = sum2/m_observations.size();

		m_Comms.Notify("ACOMMS_SCHEDULER_OFFSET", m_meanOffset);
		m_Comms.Notify("ACOMMS_SCHEDULER_DURATION", m_meanDuration);

		m_dataComplete = false;
		m_receiveComplete = false;
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AcommsScheduler::OnConnectToServer() {
	m_MissionReader.GetConfigurationParam("period", m_period);
	m_MissionReader.GetConfigurationParam("pre_lock", m_preLock);
	m_MissionReader.GetConfigurationParam("post_lock", m_postLock);
	m_MissionReader.GetConfigurationParam("source_ID", m_sourceID);

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
	// don't do anything if we don't have at least one observation
	if (m_observations.size()<1) {
		setLock(false);
		updateState(unset);
		return true;
	}

	// determine current slot and offset
	double m_time = JoshUtil::getSystemTimeSeconds();
	double m_offset = modf(m_time/m_period, &m_slot)*m_period - m_meanOffset;
	if (m_offset < 0) m_offset+=m_period;
	std::cout << "offset is " << m_offset << std::endl;

	// determine current state within slot
	if (0<m_offset && m_offset<=m_meanDuration) {
		updateState(lock);
	} else if (m_meanDuration<m_offset && m_offset<=m_meanDuration+m_postLock) {
		updateState(post_lock);
	} else if (m_period-m_preLock<m_offset && m_offset<=m_period) {
		updateState(pre_lock);
	} else {
		updateState(unlocked);
	}

	// set lock according to state
	switch (m_state) {
	case lock:
		setLock(true);
		break;

	case post_lock:
		setLock(true);
		break;

	case pre_lock:
		setLock(true);
		break;

	case unlocked:
		setLock(false);
		break;

	default:
		break;
	}

	if (m_haveTransmission && m_state==unlocked && m_driverStatus==HoverAcomms::READY) {
		switch (m_transmissionSource) {
		case ACOMMS_TRANSMIT_DATA:
			m_Comms.Notify("SCHEDULER_TRANSMIT_DATA", m_transmissionData);
			break;

		case ACOMMS_TRANSMIT_DATA_BINARY:
			m_Comms.Notify("SCHEDULER_TRANSMIT_DATA_BINARY", (void*) m_transmissionData.data(), m_transmissionData.size());
			break;

		case ACOMMS_TRANSMIT:
			m_Comms.Notify("SCHEDULER_TRANSMIT",  (void*) m_transmissionData.data(), m_transmissionData.size());
			break;

		default:
			break;
		}
		m_haveTransmission = false;
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AcommsScheduler::OnStartUp() {
	RegisterVariables();
	return (true);
}

void AcommsScheduler::updateState(STATE state) {
	if (state!=m_state) {
		m_state = state;
		postState();
	}
}

void AcommsScheduler::postState() {
	m_Comms.Notify("ACOMMS_SCHEDULER_STATE", m_state);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void AcommsScheduler::RegisterVariables() {
	m_Comms.Register("ACOMMS_TRANSMIT_DATA", 0);
	m_Comms.Register("ACOMMS_TRANSMIT_DATA_BINARY", 0);
	m_Comms.Register("ACOMMS_TRANSMIT", 0);

	m_Comms.Register("ACOMMS_RECEIVED", 0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS", 0);
}

void AcommsScheduler::setLock(bool lock) {
	// only set if different from current setting
	if (m_lockEnabled != lock) {
		if (lock)
			m_Comms.Notify("ACOMMS_TRANSMIT_LOCKOUT", 1.0);
		else
			m_Comms.Notify("ACOMMS_TRANSMIT_LOCKOUT", 0.0);
	}
	m_lockEnabled = lock;
}
