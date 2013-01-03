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

	double m_osx, m_osy;

	double m_lastMarkerPostTime;

	std::vector<double> m_follower_range_divs;

    void drawTarget( double x, double y );
    void drawMarker( std::string type, double x, double y,
            std::string label, std::string msg, std::string color );
};

#endif 
