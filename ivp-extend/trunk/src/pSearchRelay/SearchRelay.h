/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SearchRelay.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef SearchRelay_HEADER
#define SearchRelay_HEADER

#include "MOOSLib.h"
#include <map>
#include <gsl/gsl_statistics_double.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <iostream>
#include "XYSegList.h"
#include <math.h>

namespace pt = boost::posix_time;

class SearchRelay : public CMOOSApp
{
public:
	SearchRelay();
	virtual ~SearchRelay();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void ComputeIndices();
	void UpdateStats(double);
	void GetWaypoints();
	std::string getRandomString(int);
	unsigned int closest_vertex(double, double);

protected:
	// insert local vars here
	//all
	std::string my_role;
	pt::ptime last,now;

	//relay
	std::string mode;
	std::map<double, std::vector<double> > data;
	std::map<double,double> mean, var;
	std::vector<double> normal_indices_five, normal_indices_one;
	std::vector<double> wpx, wpy;
	std::string relay_message;
	double fudge_factor;
	XYSegList seglist;
	double myx,myy;
	double targetx,targety;
	bool waiting, relaying, transmit_success;

	//shore
	pt::time_duration wait_time;
	int rate,counter;
	std::string relay_status,end_status;
	std::string acomms_driver_status, start;
	//end
};

#endif
