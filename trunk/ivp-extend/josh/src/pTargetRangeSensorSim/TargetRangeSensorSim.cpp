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
	m_LastTargetMarkTime = 0;
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
      std::string key = msg.GetKey();
      if ( key == "TARGET_RANGE_REQUEST" ) {
    	  handleRangeRequest( msg.GetString() );
      } else if ( key == "TARGET_SIM_COMMAND" ) {
    	  std::string cmd = msg.GetString();
    	  MOOSToUpper(cmd);
    	  if (cmd == "PAUSE")
    		  m_sim.pause();
    	  else if (cmd == "RESUME")
    		  m_sim.resume();
    	  else if (cmd == "RESET")
    		  m_sim.reset();
      }
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TargetRangeSensorSim::OnConnectToServer()
{
	std::string sim_type;
	m_MissionReader.GetConfigurationParam("sim_type", sim_type);
	if ( sim_type == "circle" ) {
		m_sim = CircleSim();
	}
	m_sim.setConfiguration(m_MissionReader);
   // register for variables here
   // possibly look at the mission file?
//   m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	m_Comms.Register("TARGET_RANGE_REQUEST", 0);
	m_Comms.Register("TARGET_SIM_COMMAND", 0);
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool TargetRangeSensorSim::Iterate()
{
	m_sim.doWork(MOOSTime());
	if ( MOOSTime() - m_LastTargetMarkTime > 1 ) {
		std::pair<double,double> loc = getTargetPos();
		drawTarget( loc.first, loc.second );
	}
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
	ss << "Target: " << (int) x << " " << (int) y;
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

std::pair<double, double> TargetRangeSensorSim::getTargetPos() {
	std::pair<double, double> pos;
	m_sim.getTargetPos( pos.first, pos.second );
	return pos;
}

double TargetRangeSensorSim::getRange( double nav_x, double nav_y ) {
	std::pair<double, double> target_pos = getTargetPos();
	return sqrt( pow(nav_x-target_pos.first, 2) + pow(nav_y-target_pos.second, 2) );
}

double TargetRangeSensorSim::getRange( double nav_x, double nav_y,
		double & target_x, double & target_y ) {
	std::pair<double, double> target_pos = getTargetPos();
	target_x = target_pos.first;
	target_y = target_pos.second;
	return sqrt( pow(nav_x-target_pos.first, 2) + pow(nav_y-target_pos.second, 2) );
}

void TargetRangeSensorSim::handleRangeRequest( std::string msg ) {
	double target_x, target_y;
	RangeSensorTypes::RangeRequest request( msg );
	MOOSToUpper(request.vname);
	double range = getRange( request.nav_x, request.nav_y, target_x, target_y );

	RangeSensorTypes::RangeReply reply;
	reply.vname = request.vname;
	reply.range = range;

	m_Comms.Notify("TARGET_RANGE_RETURN_" + request.vname, reply.toString() );

	drawDistance( request.nav_x, request.nav_y,
			target_x, target_y,
			range, request.vname );

}
