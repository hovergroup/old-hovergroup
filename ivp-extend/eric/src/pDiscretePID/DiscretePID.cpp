/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: DiscretePID.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "DiscretePID.h"
//---------------------------------------------------------
// Constructor

DiscretePID::DiscretePID()
{
	alpha = 0;
	beta = 0;
	gamma = 0;
	delta = 0;
	epsilon = 1;
	tau = 0.1;

	desired_heading = 0;
	current_compass = 0;

	error_history.push_back(0);
	error_history.push_back(0);
	error_history.push_back(0);

	command_history.push_back(0);
	command_history.push_back(0);
}

//---------------------------------------------------------
// Destructor

DiscretePID::~DiscretePID()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DiscretePID::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();

		if(key=="DESIRED_HEADING_ERIC"){
			desired_heading = msg.GetDouble();
			desired_heading = angle180(desired_heading);
		}
		else if(key=="COMPASS_HEADING_FILTERED"){
			current_compass = msg.GetDouble();
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DiscretePID::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_Comms.Notify("DESIRED_HEADING_ERIC",0);

	m_MissionReader.GetConfigurationParam("Alpha", alpha);
	m_MissionReader.GetConfigurationParam("Beta", beta);
	m_MissionReader.GetConfigurationParam("Gamma", gamma);
	m_MissionReader.GetConfigurationParam("Delta", delta);
	m_MissionReader.GetConfigurationParam("Epsilon", epsilon);
	m_MissionReader.GetConfigurationParam("Tau",tau);
	m_MissionReader.GetConfigurationParam("IHeading",desired_heading);

	m_Comms.Notify("DESIRED_HEADING",desired_heading);
	m_Comms.Register("DESIRED_HEADING_ERIC",0);
	m_Comms.Register("COMPASS_HEADING_FILTERED",0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool DiscretePID::Iterate()
{
	cout << endl;
	cout << "Command History: " << command_history[1] << "," << command_history[0] << endl;

	cout << endl;
	cout << "Error History: " << error_history[2] << "," << error_history[1]<< "," << error_history[0] << endl;

	// happens AppTick times per second
	double rudder = getRudder();
	command_history[1] = command_history[0];
	command_history[0] = rudder;

	rudder *= -1;
	cout << rudder << endl;
	m_Comms.Notify("DESIRED_RUDDER",rudder);

	error_history[2] = error_history[1];
	error_history[1] = error_history[0];
	error_history[0] = current_compass - desired_heading;
	error_history[0] = angle180(error_history[0]);

	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool DiscretePID::OnStartUp()
{
	// happens before connection is open

	return(true);
}

double DiscretePID::getRudder()
{

	double rudder = (1/epsilon) * (alpha*error_history[2] + beta*error_history[1] + tau*error_history[0] - gamma*command_history[1] - delta*command_history[0]);

	if(rudder >= 45){rudder = 45;}
	if(rudder <= -45){rudder = -45;}

	return rudder;
}
