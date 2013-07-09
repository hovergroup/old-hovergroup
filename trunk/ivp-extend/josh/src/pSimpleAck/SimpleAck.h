/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SimpleAck.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef SimpleAck_HEADER
#define SimpleAck_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "HoverAcomms.h"
#include <vector>

class SimpleAck : public CMOOSApp
{
public:
	SimpleAck();
	virtual ~SimpleAck();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	double DELAY;

	HoverAcomms::AcommsReception m_reception;
	double m_ackTime;
	bool m_modemReady;

	void goodAck();
	void badAck();
	void partialAck(int badframes);
};

#endif 


// VIEW_SEGLIST="label,blah:msg,showthis:x,y:x,y"
