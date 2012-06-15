/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Eric.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Eric_HEADER
#define Eric_HEADER

#include "MOOSLib.h"

class Eric : public CMOOSApp
{
public:
	Eric();
	virtual ~Eric();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
