/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RoundRobin.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RoundRobin_HEADER
#define RoundRobin_HEADER

#include "MOOSLib.h"

class RoundRobin : public CMOOSApp
{
public:
	RoundRobin();
	virtual ~RoundRobin();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
