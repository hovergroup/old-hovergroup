/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RandomTransmit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RandomTransmit_HEADER
#define RandomTransmit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <sstream>

class RandomTransmit : public CMOOSApp
{
public:
	RandomTransmit();
	virtual ~RandomTransmit();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	bool m_modemReady, m_request;
	int m_requestIndex;

	int maxLength;

	std::string getRandomString(int length);
};

#endif 


// VIEW_SEGLIST="label,blah:msg,showthis:x,y:x,y"
