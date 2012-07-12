/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: rateless_coding.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef rateless_coding_HEADER
#define rateless_coding_HEADER

#include "MOOSLib.h"
#include "acomms_messages.h"
#include <vector>
#include <stringstream>

using namespace std;

class rateless_coding : public CMOOSApp
{
public:
	rateless_coding();
	virtual ~rateless_coding();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	double last_time, total_frames, successful_frames;
	double counter, duty_cycle;
	bool paused, driver_ready;
	vector<string> rateless_data;
	int rateless_index, min_frames;
	bool waiting,received;

};

#endif 
