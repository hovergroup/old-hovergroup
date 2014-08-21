/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Witsen.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "Witsen.h"

using namespace std;
//---------------------------------------------------------
// Constructor

Witsen::Witsen() : generator(boost::mt19937(time(0)),boost::normal_distribution<>())
{
	nav_x = 0;
	nav_y = 0;
	sim_x = 0;
	sim_y = 0;
	targ_origin_x = 10;
	targ_origin_y = 0;
	targ_x = 0;
	targ_y = 0;
	x_desired = 0;
	y_desired = 0;
	x = 0;
	u = 0;
	delta = 1;
	sigma = 5;
	standoff = 5;
	command = "reset";
	srand(time(0));
}

//---------------------------------------------------------
// Destructor

Witsen::~Witsen()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Witsen::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();

		if (key == "NAV_X") {
			nav_x = msg.GetDouble();
		} else if (key == "NAV_Y") {
			nav_y = msg.GetDouble();
		} else if(key == "TARG_X") {
			targ_x = msg.GetDouble();
		} else if(key == "TARG_Y"){
			targ_y = msg.GetDouble();
		} else if(key == "MISSION_COMMAND"){
			command = msg.GetString();
		}

	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Witsen::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_Comms.Register("NAV_X", 0);
	m_Comms.Register("NAV_Y", 0);
	m_Comms.Register("TARG_X", 0);
	m_Comms.Register("TARG_Y", 0);
	m_Comms.Register("MISSION_COMMAND",0);

	m_MissionReader.GetConfigurationParam("ID",id);
	m_MissionReader.GetConfigurationParam("Delta",delta);
	m_MissionReader.GetConfigurationParam("Standoff",standoff);
	m_MissionReader.GetConfigurationParam("Sigma", sigma);
	m_MissionReader.GetConfigurationParam("Targ_Origin_X", targ_origin_x);
	m_MissionReader.GetConfigurationParam("Targ_Origin_Y", targ_origin_y);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Witsen::Iterate()
{
	// happens AppTick times per second

	if(command=="go"){
		if(id==1){
			//Simulate Target
			sim_x = targ_origin_x + sigma*generator();
			sim_y = targ_origin_y + sigma*generator();

			stringstream ss;
			ss << "type=" << "diamond";
			ss << ",x=" << sim_x;
			ss << ",y=" << sim_y;
			ss << ",label=" << "target";
			ss << ",COLOR=" << "orange";
			ss << ",msg=" << "Target: " << (int) sim_x << " " << (int) sim_y;;
			m_Comms.Notify("VIEW_MARKER",ss.str());

			//Compute Control
			x = sqrt(pow((nav_x-sim_x),2) + pow((nav_y-sim_y),2));
			double ndelta = (x-standoff)/delta;
			double quadrant = fmod(ndelta,3);
			if(quadrant > 1 && quadrant < 2){
				int ufloor = floor(ndelta/3);
				u = -3*(quadrant-1) - 3*ufloor;
				cout << "Grad: "<< x << "," << ndelta << ","<<quadrant<< "," << u << endl;
			}
			else{
				int uround = round(ndelta/3);
				u = -3*uround;
				cout << "Flat: " << x << "," << ndelta << "," << u << endl;
			}

			double angle = atan2(nav_x-sim_x,nav_y-sim_y);
			x_desired = nav_x + u*sin(angle);
			y_desired = nav_y + u*cos(angle);

			ss.str("");
			ss << "type=" << "circle";
			ss << ",x=" << x_desired;
			ss << ",y=" << y_desired;
			ss << ",label=" << "control";
			ss << ",COLOR=" << "blue";
			ss << ",msg=" << "Control1: " << (int) x_desired << " " << (int) y_desired;
			m_Comms.Notify("VIEW_MARKER",ss.str());

			stringstream update;
			update <<"points="<< nav_x << "," << nav_y << ":" << x_desired << "," << y_desired;
			//cout << update.str() << endl;
			m_Comms.Notify("GOTO_UPDATES",update.str());
		}
		else{
			//Compute Control
			x = sqrt(pow((nav_x-targ_x),2) + pow((nav_y-targ_y),2));
			double ndelta = (x-standoff)/delta;
			double quadrant = fmod(ndelta,3);
			if(quadrant > 1 && quadrant < 2){

				cout << "Grad: "<< x << "," << ndelta << ","<<quadrant<< "," << u << endl;
			}
			else{

				cout << "Flat: " << x << "," << ndelta << "," << u << endl;
			}

			double angle = atan2(nav_x-sim_x,nav_y-sim_y);
			x_desired = nav_x + u*sin(angle);
			y_desired = nav_y + u*cos(angle);

			stringstream ss;
			ss << "type=" << "circle";
			ss << ",x=" << x_desired;
			ss << ",y=" << y_desired;
			ss << ",label=" << "control";
			ss << ",COLOR=" << "red";
			ss << ",msg=" << "Control2: " << (int) x_desired << " " << (int) y_desired;
			m_Comms.Notify("VIEW_MARKER",ss.str());

			stringstream update;
			update <<"points="<< nav_x << "," << nav_y << ":" << x_desired << "," << y_desired;
			//cout << update.str() << endl;
			m_Comms.Notify("GOTO_UPDATES",update.str());
		}
	}
	else if(command=="reset"){
		if(id==1){
			double angle = atan2(nav_x-targ_origin_x,nav_y-targ_origin_y);
			x_desired = nav_x + u*sin(angle);
			y_desired = nav_y + u*cos(angle);

			stringstream update;
			update <<"points="<< nav_x << "," << nav_y << ":" << x_desired << "," << y_desired;
			//cout << update.str() << endl;
			m_Comms.Notify("GOTO_UPDATES",update.str());
		}
		else{

		}
	}
	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Witsen::OnStartUp()
{
	// happens before connection is open

	return(true);
}

