/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Eric.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Eric_HEADER
#define Eric_HEADER

#include "MOOSLib.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>

using namespace std;
namespace pt = boost::posix_time;

class Eric : public CMOOSApp
{
public:
	Eric();
	virtual ~Eric();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	bool transmit;
	double heading;
	pt::time_duration wait_time;
	pt::ptime last,now;
	vector<double> x,y;
	double mlast;
	string role;
};

#endif 
