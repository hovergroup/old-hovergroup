/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TargetRangeSensorSim.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TargetRangeSensorSim_HEADER
#define TargetRangeSensorSim_HEADER

#include "MOOSLib.h"

class TargetRangeSensorSim : public CMOOSApp
{
public:
	TargetRangeSensorSim();
	virtual ~TargetRangeSensorSim();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	std::pair<double,double> getTargetPos();
	double getRange( double nav_x, double nav_y );

	void drawTarget( double x, double y );
	void drawDistance( double nav_x, double nav_y,
						double target_x, double target_y,
						double range, std::string vehicle );

	void drawMarker( std::string type, double x, double y,
			std::string label, std::string msg, std::string color );
	void drawSeglist( std::string label, std::string msg,
			std::vector< std::pair<double, double> > points );

	double m_LastTargetMarkTime;
};

#endif 


// VIEW_SEGLIST="label,blah:msg,showthis:x,y:x,y"
