/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ScheduledTransmit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "ScheduledTransmit.h"
//---------------------------------------------------------
// Constructor

ScheduledTransmit::ScheduledTransmit() {
	m_lastSentSlot = -1;
	enabled = false;
}

//---------------------------------------------------------
// Destructor

ScheduledTransmit::~ScheduledTransmit() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ScheduledTransmit::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		if (msg.GetKey()=="SCHEDULED_TRANSMITS") {
			if (MOOSToUpper(msg.GetString())=="ON") {
				enabled = true;
			} else if (MOOSToUpper(msg.GetString())=="OFF") {
				enabled = false;
			}
		}
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ScheduledTransmit::OnConnectToServer() {
	bool ok1, ok2;
	ok1 = m_MissionReader.GetConfigurationParam("period", m_period);
	ok2 = m_MissionReader.GetConfigurationParam("offset", m_offset);

	if (!ok1 || !ok2) {
		std::cout << "Must define period and offset in configuration file."
				<< std::endl;
		exit(1);
	}

	m_Comms.Register("SCHEDULED_TRANSMITS",0);

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool ScheduledTransmit::Iterate() {
	if (m_lastSentSlot == -1) {
		m_lastSentSlot = getNextSlot()-1;
	}

	if (getTime() > getSlotTime(m_lastSentSlot+1)) {
		if (enabled)
			post();

		m_lastSentSlot = getNextSlot()-1;
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ScheduledTransmit::OnStartUp() {
	// happens before connection is open

	return (true);
}

void ScheduledTransmit::post() {
	std::stringstream ss;
	ss << m_lastSentSlot++ << "---";
	ss << getRandomString(2048); // max packet size (psk rate 5)
	m_Comms.Notify("ACOMMS_TRANSMIT_DATA", ss.str());
}

double ScheduledTransmit::getTime() {
	return boost::posix_time::microsec_clock::local_time().
			time_of_day().total_milliseconds()/1000.0;
}

int ScheduledTransmit::getNextSlot() {
	return ceil((getTime()-m_offset)/m_period);
}

double ScheduledTransmit::getSlotTime(int slot) {
	return m_period*slot + m_offset;
}


std::string ScheduledTransmit::getRandomString(int length) {
	srand((unsigned) time(NULL));

	std::stringstream ss;
	const int passLen = length;
	for (int i = 0; i < passLen; i++) {
		char num = (char) ( rand() % 62 );
		if ( num < 10 )
			num += '0';
		else if ( num < 36 )
			num += 'A'-10;
		else
			num += 'a'-36;
		ss << num;
	}

	return ss.str();
}
