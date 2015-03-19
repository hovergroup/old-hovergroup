/*
 * mc2922
 *        File: PointAssign.cpp
 *  Created on: Mar 18, 2015
 *      Author: mc2922
 */

#include <iterator>
#include "MBUtils.h"
#include "PointAssign.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
	assign_by_region = false;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();
		if (key == "VISIT_POINT") {
			string val = msg.GetString();

			string mysub = MOOSChomp(val,","); //chomp out x
			if(mysub=="firstpoint"){
				//Do nothing
			}
			else if(mysub=="lastpoint"){
				//Assign points now
				stringstream msgOut;
				if(assign_by_region){// Assign by region
					m_Comms.Notify("VISIT_POINT_HENRY","firstpoint");
					m_Comms.Notify("VISIT_POINT_GILDA","firstpoint");

					for(int i=0;i<100;i++){
						msgOut<<"x="<<xvec[i]<<", "<<"y="<<yvec[i]<<", "<<"id="<<idvec[i];
						if(xvec[i]>112.5){ // Henry East (112.5 is the midpoint of the region)
						m_Comms.Notify("VISIT_POINT_HENRY",msgOut.str());
						//cout << "Henry: "<< msgOut.str() << endl;
						}else{ //Gilda West
						m_Comms.Notify("VISIT_POINT_GILDA",msgOut.str());
						//cout << "Gilda: "<< msgOut.str() << endl;
						}
						msgOut.str("");
					}
					m_Comms.Notify("VISIT_POINT_HENRY","lastpoint");
					m_Comms.Notify("VISIT_POINT_GILDA","lastpoint");
				}else{ //Alternating assignment
					m_Comms.Notify("VISIT_POINT_HENRY","firstpoint");
					m_Comms.Notify("VISIT_POINT_GILDA","firstpoint");
					for(int i=1;i<100;i++){
						msgOut<<"x="<<xvec[i-1]<<", "<<"y="<<yvec[i-1]<<", "<<"id="<<idvec[i-1];
						m_Comms.Notify("VISIT_POINT_HENRY",msgOut.str());
						//cout << "Henry: "<< msgOut.str() << endl;

						msgOut.str("");
						msgOut<<"x="<<xvec[i]<<", "<<"y="<<yvec[i]<<", "<<"id="<<idvec[i];
				        m_Comms.Notify("VISIT_POINT_GILDA",msgOut.str());
						//cout << "Gilda: "<< msgOut.str() << endl;
					}
					m_Comms.Notify("VISIT_POINT_HENRY","lastpoint");
					m_Comms.Notify("VISIT_POINT_GILDA","lastpoint");
				}
			}
			else{
				MOOSChomp(mysub,"=");
				xvec.push_back(boost::lexical_cast<double>(mysub));
				mysub = MOOSChomp(val,","); //Chomp ystr
				MOOSChomp(mysub,"=");
				yvec.push_back(boost::lexical_cast<double>(mysub));
				MOOSChomp(val,"=");
				idvec.push_back(boost::lexical_cast<double>(val));
			}
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
	m_Comms.Register("VISIT_POINT", 0);
	return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
	return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
	string config_val;
	m_MissionReader.GetConfigurationParam("assign_by_region", config_val);

	if(config_val=="true"){
		assign_by_region =true;
	}else{
		assign_by_region = false;
	}

	return true;
}

