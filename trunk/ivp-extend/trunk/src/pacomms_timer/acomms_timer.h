/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_timer.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef acomms_timer_HEADER
#define acomms_timer_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <sstream>
#include <acomms_messages.h>

class acomms_timer : public CMOOSApp
{
public:
	acomms_timer();
	virtual ~acomms_timer();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	std::string getRandomString(int);
	int getPacketSize(int);

protected:
	// insert local vars here
	double duty_cycle;
	double last_time;
	bool paused;
	bool driver_ready;
	std::string mode;
	std::string data_out;
	int rate,counter,size, pong_rate;
};

#endif 
