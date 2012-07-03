/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayStart.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RelayStart_HEADER
#define RelayStart_HEADER

#include "MOOSLib.h"

class RelayStart : public CMOOSApp
{
public:
	RelayStart();
	virtual ~RelayStart();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
