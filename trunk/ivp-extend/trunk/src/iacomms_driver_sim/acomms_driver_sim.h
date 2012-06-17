/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver_sim.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef acomms_driver_sim_HEADER
#define acomms_driver_sim_HEADER

#include "MOOSLib.h"

class acomms_driver_sim : public CMOOSApp
{
public:
	acomms_driver_sim();
	virtual ~acomms_driver_sim();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
