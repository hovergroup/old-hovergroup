/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TulipTarget.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TulipTarget_HEADER
#define TulipTarget_HEADER

#include "MOOSLib.h"
#include "TimedAcomms.h"
#include "goby/acomms/connect.h"
#include "RangeSensorTypes.h"

class TulipTarget: public CMOOSApp {
public:
	TulipTarget();
	virtual ~TulipTarget();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	double m_osx_minimum, m_osx_maximum;
	double m_osy_minimum, m_osy_maximum;

	TimedAcomms m_AcommsTimer;

	void handleDebug( const std::string msg );
	void handleUpdate( const std::string msg );

	void onTransmit();

	std::string m_AcommsStatus;
	std::string m_ReceivedData;
	bool m_WaitingForData;
	double m_ReceivedDataTime, m_BadFramesTime;

	double m_osx, m_osy, m_target_range;
	double m_set_x, m_set_y;

	double m_lastRangeRequestTime;
	std::string m_name;

	std::vector<double> m_follower_range_divs;
};

#endif 
