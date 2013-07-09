/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Relay.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Relay_HEADER
#define Relay_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "HoverAcomms.h"
#include "math.h"

enum RelayState {
	idle = 0,
	link1,
	transmit,
	link2
};

class Relay : public CMOOSApp
{
public:
	Relay();
	virtual ~Relay();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	double RELAY_DELAY, REPEAT_DELAY;
	int SOURCE, DEST;
	double LINK1_TIMEOUT, LINK2_TIMEOUT;
	double MATLAB_RADIUS;

	HoverAcomms::AcommsReception m_reception;

	RelayState m_state;
	bool m_enable, m_modemReady, m_gotAck, m_gotTrans, m_transGood, m_ackGood;
	std::string m_relayData;

	double m_x, m_y, m_setX, m_setY;
	double m_lastTime, m_initX, m_initY, m_relayX, m_relayY;
	int m_iteration;

	void initCycle();
	void reportFailure();
	void reportSuccess();
	void doRelay();

	double dist(double x1, double y1, double x2, double y2);
	std::string formatResult(int iter, double x1, double y1, double x2, double y2, int success);
};

#endif 


// VIEW_SEGLIST="label,blah:msg,showthis:x,y:x,y"
