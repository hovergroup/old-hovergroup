/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: DiscretePID.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef DiscretePID_HEADER
#define DiscretePID_HEADER

#include "MOOSLib.h"

class DiscretePID : public CMOOSApp
{
public:
	DiscretePID();
	virtual ~DiscretePID();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
