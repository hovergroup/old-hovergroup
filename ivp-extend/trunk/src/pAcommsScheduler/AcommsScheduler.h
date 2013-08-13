/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommsScheduler.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommsScheduler_HEADER
#define AcommsScheduler_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

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
	// Configuration variables

private:
	// State variables
	unsigned int m_iterations;

	double m_period, m_preLock, m_postLock;
};

#endif 
