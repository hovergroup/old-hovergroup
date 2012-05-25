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
#include <set>

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
	std::map<std::string,int> bad_frames;
	std::map<std::string,int> good_frames;
	std::map<std::string,int> all_frames;
	std::map<std::string,int> sync;
	std::map<std::string,int> loss;
	std::map<std::string,int> success;
	std::map<std::string,int> expected;
	std::map<std::string,int> total_expected;
	std::string transmitter;
	std::set<std::string> vehicles;
};

#endif 
