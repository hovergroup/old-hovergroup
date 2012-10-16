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
	// insert local vars here
};

#endif 
