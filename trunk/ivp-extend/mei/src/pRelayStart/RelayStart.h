/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayStart.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RelayStart_HEADER
#define RelayStart_HEADER

#include "MOOSLib.h"

using namespace std;

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
		double last, wait_time;
		int rate,mail_counter,end_id,length;
		string relay_status,end_status, driver_status, pause;

		bool relay_sync;
};

#endif 
