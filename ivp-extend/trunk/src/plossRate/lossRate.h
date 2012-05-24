/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: lossRate.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef lossRate_HEADER
#define lossRate_HEADER

#include "MOOSLib.h"

class lossRate : public CMOOSApp
{
public:
	lossRate();
	virtual ~lossRate();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
