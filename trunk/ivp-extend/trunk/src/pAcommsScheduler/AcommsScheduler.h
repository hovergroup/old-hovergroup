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
	unset
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

	// learning
	HoverAcomms::DriverStatus m_driverStatus;
	int m_dataComplete;

	double m_lastStart, m_lastEnd; // last observed
	double m_meanOffset, m_meanDuration; // current mean
	std::deque<std::pair<double,double> > m_observations; // current history

};

#endif 
