/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TulipTarget.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TulipTarget_HEADER
#define TulipTarget_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "TimedAcomms.h"
#include "goby/acomms/connect.h"
#include "RangeSensorTypes.h"

class TulipWifi: public CMOOSApp {
public:
	TulipWifi();
	virtual ~TulipWifi();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	void publishData();
	std::string formatWaypoint(double x, double y);

	double m_lastUpdateTime, m_timeout;

	double m_followerX, m_followerY, m_leaderX, m_leaderY;

	double m_leader, m_follower;

	std::string m_leaderName, m_followerName;
};

#endif 
