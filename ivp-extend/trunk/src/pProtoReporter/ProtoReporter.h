/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ProtoReporter.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef ProtoReporter_HEADER
#define ProtoReporter_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "reports.pb.h"
#include "HoverAcomms.h"

class ProtoReporter: public CMOOSApp {
public:
	ProtoReporter();
	~ProtoReporter();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	// Configuration variables

private:
	double m_lastAcommsStatusUpdate, m_lastHelmStateUpdate, m_lastGPSQualityUpdate;

	double m_navX, m_navY, m_navHeading, m_navSpeed, m_navDepth;
	double m_voltage;
	bool m_acommsDriverRunning;
	std::string m_name;

	ProtoNodeReport nr;
};

#endif 
