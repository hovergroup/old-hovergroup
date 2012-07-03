/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayEnd.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RelayEnd_HEADER
#define RelayEnd_HEADER

#include "MOOSLib.h"

class RelayEnd : public CMOOSApp
{
public:
	RelayEnd();
	virtual ~RelayEnd();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
