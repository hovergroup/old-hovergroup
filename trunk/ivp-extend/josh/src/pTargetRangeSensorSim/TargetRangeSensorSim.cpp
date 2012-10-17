/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TargetRangeSensorSim.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "TargetRangeSensorSim.h"
//---------------------------------------------------------
// Constructor

TargetRangeSensorSim::TargetRangeSensorSim()
{
}

//---------------------------------------------------------
// Destructor

TargetRangeSensorSim::~TargetRangeSensorSim()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TargetRangeSensorSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TargetRangeSensorSim::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	m_Comms.Register("TARGET_RANGE_REQUEST", 0);
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool TargetRangeSensorSim::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool TargetRangeSensorSim::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

void TargetRangeSensorSim::drawTarget( double x, double y ) {
	std::stringstream ss;
	ss << "Target: " << (int) x << ", " << (int) y;
	drawMarker( "diamond", x, y, "target", ss.str(), "orange" );
}

void TargetRangeSensorSim::drawDistance( double nav_x, double nav_y,
		double target_x, double target_y, double range, std::string vehicle ) {
	std::stringstream ss;
	ss << (int) range;

	std::vector< std::pair<double, double> > points;
	points.push_back( std::pair<double,double>( nav_x, nav_y) );
	points.push_back( std::pair<double,double>( target_x, target_y) );

	drawSeglist( vehicle, ss.str(), points );
}

void TargetRangeSensorSim::drawMarker( std::string type, double x, double y,
		std::string label, std::string msg, std::string color ) {

	std::stringstream ss;
	ss << "type=" << type;
	ss << ",x=" << x;
	ss << ",y=" << y;
	ss << ",label=" << label;
	ss << ",COLOR=" << color;
	ss << ",msg=" << msg;

	m_Comms.Notify("VIEW_MARKER", ss.str() );
}

void TargetRangeSensorSim::drawSeglist( std::string label, std::string msg,
		std::vector< std::pair<double, double> > points ) {

	std::stringstream ss;
	ss << "label," << label;
	ss << ":msg," << msg;
	for ( int i=0; i<points.size(); i++ ) {
		ss << ":" << points[i].first << "," << points[i].second;
	}

	m_Comms.Notify("VIEW_SEGLIST", ss.str() );
}
