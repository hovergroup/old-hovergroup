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
	void GetWaypoints();
	std::string getRandomString(int);

protected:
	// insert local vars here
	//all
	std::string my_role;
	pt::ptime last,now;
	//relay
	std::string mode;
	std::map<double, std::vector<double>> data;
	std::map<double,double> mean, var;
	std::vector<double> normal_indices_five, normal_indices_one;
	std::vector<double> wpx, wpy;
	double fudge_factor;
	XYSegList seglist;
	double myx,myy;
	//shore
	pt::time_duration wait_time;
	int rate,counter;
	std::string relay_status,end_status;
	//end
};

#endif
