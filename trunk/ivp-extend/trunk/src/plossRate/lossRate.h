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
	std::map<std::string,double> bad_frames;
	std::map<std::string,double> good_frames;
	std::map<std::string,double> all_frames;
	std::map<std::string,double> sync;
	std::map<std::string,double> loss;
	std::map<std::string,double> success;
	std::map<std::string,double> expected;
	std::map<std::string,double> total_expected;
	std::string transmitter;
	std::set<std::string> vehicles;
};

#endif 
