/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TargetRangeSensorSim.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TargetRangeSensorSim_HEADER
#define TargetRangeSensorSim_HEADER

#include "MOOSLib.h"

class TargetRangeSensorSim : public CMOOSApp
{
public:
	TargetRangeSensorSim();
	virtual ~TargetRangeSensorSim();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 


// VIEW_SEGLIST="label,blah:msg,showthis:x,y:x,y"
