/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: NavManager.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef NavManager_HEADER
#define NavManager_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

enum NAV_SOURCE {
	gps = 0,
	rtk,
	none
};

class NavManager: public CMOOSApp {
public:
	NavManager();
	~NavManager();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	double TIMEOUT;

private:
	NAV_SOURCE source;
	bool gps_lock;
	double gps_update_time, rtk_update_time;

	void setSource(NAV_SOURCE new_val);
};

#endif 
