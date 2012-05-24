/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: lossRate.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef lossRate_HEADER
#define lossRate_HEADER

#include "MOOSLib.h"
#include <acomms_messages.h>
#include <iostream>
#include <map>

class lossRate : public CMOOSApp
{
public:
	lossRate();
	virtual ~lossRate();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here

	double timeout;
	double last_time;
	int transmit_frames;
	int frames_sent;
	bool listening;
	bool clear_old;
	std::map bad_frames;
	std::map good_frames;
	std::map all_frames;
	std::map sync;
	std::map loss;
	std::map success;
	std::map expected;
	std::map total_expected;
	std::string transmitter;
	std::set vehicles;
};

#endif 
