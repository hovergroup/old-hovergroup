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
	m_lockEnabled = false;
	m_state = unset;
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
				m_dataComplete = 0;
			} else if (m_driverStatus==HoverAcomms::RECEIVING && val==HoverAcomms::READY) {
				// end of receipt
				m_lastEnd = JoshUtil::getSystemTimeSeconds();
				// increment
				m_dataComplete++;
			}
			m_driverStatus = val;
		} else if (key=="ACOMMS_RECEIVED") {
			HoverAcomms::AcommsReception reception;
			if (reception.parseFromString(msg.GetString())) {
				if (reception.getSource()==m_sourceID)
					m_dataComplete++;
			}
		}
	}

	if (m_dataComplete==2) { // got both status timings and data output
		m_observations.push_back(std::pair<double,double>(m_lastStart,m_lastEnd));
		while (m_observations.size()>MAX_OBSERVATIONS) {
			m_observations.pop_front();
		}
		double sum1=0, sum2=0;
		for (int i=0; i<m_observations.size(); i++) {
			double slot;
			sum1 += modf(m_observations[i].first/m_period, &slot);
			sum2 += m_observations[i].second - m_observations[i].first;
		}
		m_meanOffset = sum1/m_observations.size();
		m_meanDuration = sum2/m_observations.size();

		m_dataComplete=0;
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
		return true;
	}

	// determine current slot and offset
	double m_time = JoshUtil::getSystemTimeSeconds();
	double m_offset = modf(m_time/m_period, &m_slot) - m_meanOffset;

	// determine current state within slot
	if (0<m_offset && m_offset<m_meanDuration) {
		m_state = lock;
	} else if (m_meanDuration<m_offset && m_offset<m_meanDuration+m_postLock) {
		m_state = post_lock;
	} else if (m_period-m_preLock<m_offset && m_offset<m_period) {
		m_state = pre_lock;
	} else {
		m_state = unlocked;
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
