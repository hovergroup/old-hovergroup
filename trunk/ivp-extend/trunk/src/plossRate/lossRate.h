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

	bool sent_something;
	double timeout;
	double last_time;

	int transmit_frames;
	lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info;
	lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info;

};

#endif 
