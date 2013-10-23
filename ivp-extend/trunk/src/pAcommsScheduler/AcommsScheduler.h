/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommsScheduler.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommsScheduler_HEADER
#define AcommsScheduler_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "JoshUtils.h"
#include "HoverAcomms.h"
#include <math.h>
#include <deque>

enum STATE {
	unlocked = 0,
	pre_lock,
	lock,
	post_lock,
	unset,
	pre_start
};

enum TransmissionSource {
	ACOMMS_TRANSMIT_DATA = 0,
	ACOMMS_TRANSMIT_DATA_BINARY,
	ACOMMS_TRANSMIT
};

class AcommsScheduler: public CMOOSApp {
public:
	AcommsScheduler();
	~AcommsScheduler();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	const static double MAX_OBSERVATIONS=4;

	// configuration
	double m_period, m_preLock, m_postLock;
	int m_sourceID;

	// state
	double m_slot;
	STATE m_state;
	bool m_lockEnabled;

	void setLock(bool lock);
	void updateState(STATE state);
	void postState();

	// learning
	HoverAcomms::DriverStatus m_driverStatus;
	bool m_dataComplete, m_receiveComplete;

	double m_lastStart, m_lastEnd; // last observed
	double m_meanOffset, m_meanDuration; // current mean
	std::deque<std::pair<double,double> > m_observations; // current history

	// transmissions
	bool m_haveTransmission;
	std::string m_transmissionData;
	TransmissionSource m_transmissionSource;

};

#endif 
