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
	void ComputeIndex();
	int Decision();
	void UpdateStats(double);
	void GetWaypoints();
	std::string getRandomString(int);
	unsigned int closest_vertex(double, double);
	void ComputeLossRates();

protected:
	// insert local vars here
	//all
	std::string my_role;
	pt::ptime last,now;

	//relay
	std::string mode;
	int discount,min_obs,total_points;
	std::map<double, std::vector<double> > data;
	std::vector<double> mean, stdev,indices;
	std::vector<double> normal_indices;
	std::vector<double> wpx, wpy;
	std::string relay_message;
	double fudge_factor;
	XYSegList seglist;
	double myx,myy;
	double targetx,targety;
	double link1_stat;
	bool waiting, relaying;

	//shore
	pt::time_duration wait_time;
	int rate,counter;
	std::string relay_status,end_status;
	std::string acomms_driver_status, start;
	//end
};

#endif
