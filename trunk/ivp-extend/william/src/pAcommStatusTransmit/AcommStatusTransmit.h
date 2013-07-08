/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommStatusTransmit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommStatusTransmit_HEADER
#define AcommStatusTransmit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include "RemusAMessages.h"

class AcommStatusTransmit : public CMOOSApp
{
public:
	AcommStatusTransmit();
	~AcommStatusTransmit();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();


	std::string m_vname;
	double m_period, m_offset;
	double m_vx, m_vy, m_vd, m_vb; //vehicle status (x,y,depth,bearing)

	int m_lastSentSlot;

	bool enabled;

	double getTime();
	int getNextSlot();
	double getSlotTime(int slot);

	void post();
	std::string getStatusString();


private: // Configuration variables

private: // State variables
};

#endif 
