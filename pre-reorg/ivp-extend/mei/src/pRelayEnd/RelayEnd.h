/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayEnd.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RelayEnd_HEADER
#define RelayEnd_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "HoverAcomms.h"
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
	double fudge_factor, station_factor;
	string driver_status;
	double mythrust;
	double relay_status;
	string heard;
	HoverAcomms::AcommsReception reception;
};

#endif 
