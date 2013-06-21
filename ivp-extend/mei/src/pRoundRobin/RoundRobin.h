/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RoundRobin.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RoundRobin_HEADER
#define RoundRobin_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

using namespace std;

#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "XYSegList.h"

class RoundRobin : public CMOOSApp
{
public:
	RoundRobin();
	virtual ~RoundRobin();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

	//Admin
	void handleDebug(string);
	void GetWaypoints();
	void RRGoto(double, double);
	void setLength(int);

protected:
	//Configuration Variables
	double through_transmission_delay, relay_transmission_delay, wait_time;

	//Local variables
	string action, mail, waypoints_msg, relay_mode;
	double connected, transmissions;
	vector<double> wpx, wpy;
	XYSegList seglist;
	double start_time;
	int current_point, total_points;
	int rate, transmissions_per_segment;
	double length;
};

#endif 
