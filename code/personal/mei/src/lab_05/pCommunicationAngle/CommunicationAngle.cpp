/*
 * Mei Cheung
 *        File: CommunicationAngle.cpp
 *  Created on: 
 *      Author: 
 */

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
	m_iterations = 0;
	m_timewarp   = 1;

	navx=0;navy=0;
	navdepth=0;navspeed=0;navheading=0;
	goto_x=0;goto_y=0;goto_d=0;

	surface_sound_speed = 1480;
	sound_speed_gradient = 0.016;
	water_depth = 6000;
	time_interval = 1;
	last_post_time = -1;
	collabFound=false;
}

//---------------------------------------------------------
// Destructor

CommunicationAngle::~CommunicationAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key  = msg.GetKey();

		if (key=="NAV_X"){
			navx=msg.GetDouble();
		}else if(key=="NAV_Y"){
			navy=msg.GetDouble();
		}else if(key=="NAV_DEPTH"){
			navdepth=msg.GetDouble();
		}else if(key=="NAV_HEADING"){
			navheading=msg.GetDouble();
		}else if (key=="NAV_SPEED"){
			navspeed=msg.GetDouble();
		}else if (key=="VEHICLE_NAME"){
			myname=msg.GetString();
		}else if(key=="COLLABORATOR_NAME"){
			mycollab=msg.GetString();
			RegisterCollab();
		}else if(key==mycollab+"_NAV_X"){
			collabx=msg.GetDouble();
			//Wait for at least one variable to arrive
			collabFound = true;
		}else if(key==mycollab+"_NAV_Y"){
			collaby=msg.GetDouble();
		}else if(key==mycollab+"_NAV_DEPTH"){
			collabdepth=msg.GetDouble();
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	m_MissionReader.GetConfigurationParam("surface_sound_speed", surface_sound_speed);
	m_MissionReader.GetConfigurationParam("sound_speed_gradient", sound_speed_gradient);
	m_MissionReader.GetConfigurationParam("water_depth", water_depth);
	m_MissionReader.GetConfigurationParam("time_interval", time_interval);

	RegisterVariables();
	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{
	if(collabFound){	//Wait for Collaborator First
		if (MOOSTime() - last_post_time > time_interval) {
			last_post_time = MOOSTime();

			double dz = collabdepth-navdepth;
			double dr = sqrt(pow(navx-collabx,2) + pow(navy-collaby,2));
			double L = sqrt(pow(dr,2) + pow(dz,2));

			double phi = atan(-dz/dr);
			double rmid = 0.5*L*cos(phi);
			double zmid = navdepth - 0.5*L*sin(phi);

			double z0 = -(surface_sound_speed)/sound_speed_gradient;
			double r0 = rmid - (zmid-z0)*tan(phi);

			double bigR = sqrt(pow(r0,2)+pow(z0-navdepth,2));
			double cz1 = surface_sound_speed+sound_speed_gradient*navdepth;
			double cz2 = surface_sound_speed+sound_speed_gradient*collabdepth;
			double theta0 = acos(cz1/bigR/sound_speed_gradient);

			//Check the deepest point does not hit the sea bottom
			//Deepest point either one of the vehicles (assume the vehicles cannot
			//be on the sea bottom
			//Or at the radius bigR from the center z0
			if (r0<0 || r0>L*cos(phi) || (z0+bigR)<water_depth){
				//Compute transmission loss
				double dtheta = MOOSDeg2Rad(0.05);
				double theta2 = theta0+dtheta;
				double ri1 = bigR*sin(theta0)-bigR*sin(theta2);
				double ri2 = bigR*sin(theta0+dtheta)-bigR*sin(theta2);
				double J = ri1*(ri2-ri1)/sin(theta2)/dtheta;
				double p = sqrt(fabs(cz2*cos(theta0)/cz1/J));
				double tLoss = -20*log10(p);

				//If current path exists, current location is fine
				goto_x = navx;
				goto_y = navy;
				goto_d = navdepth;

				PublishAnswer(true,MOOSRad2Deg(-theta0),tLoss);
			}
			else{
				//If the arc hits the sea bottom, give the vehicle
				//a command to decrease depth so that the ray is
				//arbitrarily chosen 5m above

				goto_x = navx;
				goto_y = navy;
				goto_d = navdepth-fabs((z0+bigR)-water_depth)-5;

				PublishAnswer(false,0,0);
			}
		}
	}
	else{
		cout<< "Collaborator Not Found" <<endl;
	}
	m_iterations++;
	return(true);
}

void CommunicationAngle::PublishAnswer(bool pathFound, double elev_angle, double transmission_loss)
{
	stringstream msgOut;
	if(pathFound){
		msgOut<<"elev_angle="<<elev_angle<<",";
		msgOut<<"transmission_loss="<<transmission_loss<<",";
	}else{
		msgOut<<"elev_angle=NaN,";
		msgOut<<"transmission_loss=NaN,";
	}
	msgOut<<"id=mc2922@mit.edu";
	m_Comms.Notify("ACOUSTIC_PATH_MC",msgOut.str());
	//cout << msgOut.str() <<endl;

	//Publish Location
	msgOut.str("");
	msgOut<<"x="<<goto_x<<",";
	msgOut<<"y="<<goto_y<<",";
	msgOut<<"depth="<<goto_d<<",";
	msgOut<<"id=mc2922@mit.edu";
	m_Comms.Notify("CONNECTIVITY_LOCATION_MC",msgOut.str());
}

bool CommunicationAngle::RegisterCollab(){
	cout << mycollab << endl;
	m_Comms.Register(mycollab+"_NAV_X",0);
	m_Comms.Register(mycollab+"_NAV_Y",0);
	m_Comms.Register(mycollab+"_NAV_DEPTH",0);
	m_Comms.Register(mycollab+"_NAV_SPEED",0);
	m_Comms.Register(mycollab+"_NAV_HEADING",0);
	return true;
}
//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle::OnStartUp()
{
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
		list<string>::iterator p;
		for(p=sParams.begin(); p!=sParams.end(); p++) {
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);

			if(param == "FOO") {
				//handled
			} else if(param == "BAR") {
				//handled
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
	return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
	m_Comms.Register("VEHICLE_NAME",0);
	m_Comms.Register("COLLABORATOR_NAME",0);
	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
	m_Comms.Register("NAV_DEPTH",0);
	m_Comms.Register("NAV_HEADING",0);
	m_Comms.Register("NAV_SPEED",0);
	m_Comms.Register("ACOUSTIC_PATH_REF",0);
	m_Comms.Register("CONNECTIVITY_LOCATION_REF",0);
}
