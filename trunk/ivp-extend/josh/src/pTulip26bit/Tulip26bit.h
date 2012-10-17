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

class Tulip26bit: public CMOOSApp {
public:
	Tulip26bit();
	virtual ~Tulip26bit();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	TimedAcomms m_AcommsTimer;

	void handleDebug( const std::string msg );
	void handleUpdate( const std::string msg );

	void onTransmit_follower();
	void onTransmit_leader();
	void onGoodReceive_follower( const std::string data );
	void onGoodReceive_leader( const std::string data );
	void onBadReceive_follower();
	void onBadReceive_leader();

	std::string m_AcommsStatus;
	std::string m_ReceivedData;
	bool m_WaitingForData;
	double m_ReceivedDataTime, m_BadFramesTime;
};

#endif 