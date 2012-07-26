/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayEnd.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RelayEnd_HEADER
#define RelayEnd_HEADER

#include "MOOSLib.h"
#include <acomms_messages.h>
#include <math.h>

using namespace std;

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
	int relay_id;
	double end_x,end_y,myx,myy;
	double fudge_factor, station_factor,now,last,update_time;
	string driver_status;
	double voltage, mythrust;
};

#endif 
