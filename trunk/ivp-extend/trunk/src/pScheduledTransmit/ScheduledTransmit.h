/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ScheduledTransmit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef ScheduledTransmit_HEADER
#define ScheduledTransmit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>

class ScheduledTransmit : public CMOOSApp
{
public:
	ScheduledTransmit();
	virtual ~ScheduledTransmit();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	double m_period, m_offset;

	int m_lastSentSlot;

	bool enabled;

	double getTime();
	int getNextSlot();
	double getSlotTime(int slot);

	void post();
	std::string getRandomString(int length);
};

#endif 
