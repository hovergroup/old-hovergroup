/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOAComms.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TDOAComms_HEADER
#define TDOAComms_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "tdoa.pb.h"
#include "tdoa_acomms.pb.h"
#include "JoshUtils.h"

class TDOAComms: public CMOOSApp {
public:
	TDOAComms();
	~TDOAComms();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	// Configuration variables
	int m_id, m_targetID;
	double m_targetOffset, m_localOffset;
	std::vector<double> m_offsets;

private:
	// State variables
	bool m_paused;

private:
	// other stuff
	TDOAUpdate m_outgoingUpdate;
	void resetOutput();
	JoshUtil::SlotFunctions m_slotFunctions;
};

#endif 
