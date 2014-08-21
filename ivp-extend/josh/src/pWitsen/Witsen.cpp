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
	sim_origin_x = 10;
	sim_origin_y = 0;
	targ_x = 0;
	targ_y = 0;
	x_desired = 0;
	y_desired = 0;
	x = 0;
	u = 0;
	delta = 1;
	sigma1 = 1;
	sigma2 = 4;
	standoff1 = 10;
	standoff2 = 20;
	sim_step = 1;
	max_dev= 10;
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
	m_MissionReader.GetConfigurationParam("Standoff1",standoff1);
	m_MissionReader.GetConfigurationParam("Standoff2",standoff2);
	m_MissionReader.GetConfigurationParam("Sigma1", sigma1);
	m_MissionReader.GetConfigurationParam("Sigma2", sigma2);
	m_MissionReader.GetConfigurationParam("Sim_Origin_X", sim_origin_x);
	m_MissionReader.GetConfigurationParam("Sim_Origin_Y", sim_origin_y);
	m_MissionReader.GetConfigurationParam("SimStep", sim_step);
	m_MissionReader.GetConfigurationParam("MaxDev", max_dev);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Witsen::Iterate()
{
	// happens AppTick times per second

	if(command=="go"){
		if(id==2){

			//targ_x = targ_x+ sigma2*generator();
			targ_y = targ_y+ sigma2*generator();

			//Compute Control
			//x = sqrt(pow((nav_x-targ_x),2) + pow((nav_y-targ_y),2));
			x = nav_y-(targ_y+standoff1);
			double ndelta = (x-standoff2)/delta;
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

			//double angle = atan2((nav_x-targ_x),(nav_y-targ_y));
			//x_desired = nav_x + u*sin(angle);
			//y_desired = nav_y + u*cos(angle);

			x_desired = targ_x;
			y_desired = nav_y+u;

			stringstream ss;
			ss << "type=" << "circle";
			ss << ",x=" << x_desired;
			ss << ",y=" << y_desired;
			ss << ",label=" << "control";
			ss << ",COLOR=" << "blue";
			ss << ",msg=" << "Control: " << (int) x_desired << " " << (int) y_desired;
			m_Comms.Notify("VIEW_MARKER",ss.str());

			stringstream update;
			update <<"points="<< nav_x << "," << nav_y << ":" << x_desired << "," << y_desired;
			//cout << update.str() << endl;
			m_Comms.Notify("MISSION_MODE","GOTO");
			m_Comms.Notify("GOTO_UPDATES",update.str());
		}
		else{
			//Simulate Target
			double guess = sigma1*generator();
			if(guess>0&&guess<2*sigma1){sim_y = sim_y + sim_step;}
			else if(guess<0&&guess>-2*sigma1){sim_y = sim_y - sim_step;}
			else if(guess<-2*sigma1){sim_y = sim_y - sigma1*sim_step;}
			else if(guess>2*sigma1){sim_y = sim_y + sigma1*sim_step;}
			if((sim_y-sim_origin_y)>max_dev){sim_y = sim_y-2*sim_step;}
			else if((sim_y-sim_origin_y)<-max_dev){sim_y = sim_y+2*sim_step;}

			stringstream ss;
			ss << "type=" << "diamond";
			ss << ",x=" << sim_x;
			ss << ",y=" << sim_y;
			ss << ",label=" << "target";
			ss << ",COLOR=" << "orange";
			ss << ",msg=" << "Target: " << (int) sim_x << " " << (int) sim_y;
			m_Comms.Notify("VIEW_MARKER",ss.str());

			//Compute Control
			//x = sqrt(pow((nav_x-sim_x),2) + pow((nav_y-sim_y),2));
			x = nav_y-sim_y;
			double ndelta = (x+standoff1)/delta;
			double quadrant = fmod(ndelta,3);
			if(ndelta<0){
				quadrant = -fmod(-ndelta,3);}
			if(abs(quadrant) > 1 && abs(quadrant) < 2){
				if(quadrant<0){u = 1.5+quadrant;}
				else{u = -1.5+quadrant;}
				cout << "Steep: "<< x << "," << ndelta << ","<<quadrant<< "," << u << endl;
			}
			else if(abs(quadrant)==2){u=quadrant;
			cout << "Steep: "<< x << "," << ndelta << ","<<quadrant<< "," << u << endl;}
			else if(abs(quadrant)>2){
				if(quadrant<0){u = 1.5+quadrant;}
				else{u = -1.5+quadrant;}
				cout << "Steep: "<< x << "," << ndelta << ","<<quadrant<< "," << u << endl;
			}
			else{
				u = -delta*quadrant;
				cout << "Flat: " << x << "," << ndelta << "," << u << endl;
			}

			x_desired = sim_x;
			y_desired = nav_y + u;

			ss.str("");
			ss << "type=" << "circle";
			ss << ",x=" << x_desired;
			ss << ",y=" << y_desired;
			ss << ",label=" << "control";
			ss << ",COLOR=" << "blue";
			ss << ",msg=" << "Control: " << (int) x_desired << " " << (int) y_desired;
			m_Comms.Notify("VIEW_MARKER",ss.str());

			stringstream update;
			update <<"points="<< nav_x << "," << nav_y << ":" << x_desired << "," << y_desired;
			//cout << update.str() << endl;
			m_Comms.Notify("MISSION_MODE","GOTO");
			m_Comms.Notify("GOTO_UPDATES",update.str());
		}
	}
	else if(command=="reset"){
		if(id==1){
			x_desired = sim_origin_x;
			y_desired = sim_origin_y - standoff1;
			sim_x = sim_origin_x;
			sim_y = sim_origin_y;

			stringstream update;
			update <<"points="<< nav_x << "," << nav_y << ":" << x_desired << "," << y_desired;
			//cout << update.str() << endl;
			m_Comms.Notify("MISSION_MODE","GOTO");
			m_Comms.Notify("GOTO_UPDATES",update.str());

			stringstream ss;
			ss << "type=" << "diamond";
			ss << ",x=" << sim_origin_x;
			ss << ",y=" << sim_origin_y;
			ss << ",label=" << "target";
			ss << ",COLOR=" << "orange";
			ss << ",msg=" << "Target: " << (int) sim_x << " " << (int) sim_y;;
			m_Comms.Notify("VIEW_MARKER",ss.str());
		}else{
			x_desired = targ_x;
			y_desired = targ_y + standoff2;

			stringstream update;
			update <<"points="<< nav_x << "," << nav_y << ":" << x_desired << "," << y_desired;
			//cout << update.str() << endl;
			m_Comms.Notify("MISSION_MODE","GOTO");
			m_Comms.Notify("GOTO_UPDATES",update.str());

//			stringstream ss;
//			ss << "type=" << "diamond";
//			ss << ",x=" << targ_x;
//			ss << ",y=" << targ_y;
//			ss << ",label=" << "target";
//			ss << ",COLOR=" << "orange";
//			ss << ",msg=" << "Target: " << (int) targ_x << " " << (int) targ_y;;
//			m_Comms.Notify("VIEW_MARKER",ss.str());
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

