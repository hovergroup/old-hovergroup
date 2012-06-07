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
#include <acomms_messages.h>
#include <boost/date_time/posix_time/posix_time.hpp>


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
	void UpdateStats(std::vector<double>);


protected:
	// insert local vars here
	//all
	std::string my_role;
	pt::ptime last,now;
	//relay
	double mean, var;
	bool travelling;
	std::vector<double> data;
	//shore
	std::string mail;
	pt::time_duration wait_time;
	//end
};

#endif
