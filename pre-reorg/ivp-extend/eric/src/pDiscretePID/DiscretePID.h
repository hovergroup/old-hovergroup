/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: DiscretePID.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef DiscretePID_HEADER
#define DiscretePID_HEADER

#include "MOOSLib.h"
#include <vector>
#include "AngleUtils.h"

using namespace std;

class DiscretePID : public CMOOSApp
{
public:
	DiscretePID();
	virtual ~DiscretePID();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

	double getRudder();

protected:
	// insert local vars here

	double alpha,beta,gamma,delta,epsilon, tau;
	double desired_heading;
	double current_compass;
	double rudder_offset;
	vector<double> error_history, command_history;

};

#endif 
