/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Tulip26bit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Tulip26bit_HEADER
#define Tulip26bit_HEADER

#include "MOOSLib.h"
#include "TimedAcomms.h"
#include "goby/acomms/connect.h"
#include "RangeSensorTypes.h"

class Tulip26bit: public CMOOSApp {
public:
	Tulip26bit();
	virtual ~Tulip26bit();

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

	void onTransmit_follower();
	void onTransmit_leader();
	void onGoodReceive_follower( const std::string data );
	void onGoodReceive_leader( const std::string data );
	void onBadReceive_follower();
	void onBadReceive_leader();
	void publishLeaderPos();

	unsigned char LinearEncode( double val, double min, double max, int bits );
	double LinearDecode( unsigned char val, double min, double max, int bits );

    unsigned char FlexibleEncode(double val, std::vector<double> & range_divs,
            int bits);

	std::string m_AcommsStatus;
	std::string m_ReceivedData;
	std::string m_FrameStatus;
	bool m_gotFrames, m_gotData;
	double m_ReceivedDataTime, m_BadFramesTime;

	double m_osx, m_osy, m_target_range;
	double m_set_x, m_set_y;

	double m_lastRangeRequestTime;
	std::string m_name;

	std::string m_range_source;
	int m_target_acomms_id;
	bool m_gotSource, m_gotRange;
	int m_sourceID, m_acommsRange;

	std::vector<double> m_follower_range_divs;
};

#endif 
