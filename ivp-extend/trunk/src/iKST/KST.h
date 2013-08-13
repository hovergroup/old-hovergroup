/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: KST.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef KST_HEADER
#define KST_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class KST: public CMOOSApp {
public:
	KST();
	~KST();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	static const char delim = ',';

	double m_navx, m_navy, m_navspeed, m_startTime;

	std::ofstream out;

	void printHeader();
	void printLine();

};

#endif 
