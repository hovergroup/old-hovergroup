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

private:
	// State variables
	int m_id;

	TDOAUpdate m_outgoingUpdate;

	void resetOutput();
};

#endif 
