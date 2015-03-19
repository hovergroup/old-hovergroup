/*
 * mc2922
 *        File: GenPath.cpp
 *  Created on: Mar 18, 2015
 *      Author: mc2922
 */

#include <iterator>
#include "MBUtils.h"
#include "GenPath.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == myvar) {
        	string val = msg.GetString();

        	string mysub = MOOSChomp(val,","); //chomp out x
        	cout << mysub << endl;
        	if(mysub=="firstpoint"){
        		//Do nothing
        	}else if(mysub=="lastpoint"){
        		//Finished - Generate a path
        		solveTSP();
        	}else{
        		MOOSChomp(mysub,"=");
        		cout << mysub << endl;
        		xvec.push_back(boost::lexical_cast<double>(mysub));
        		mysub = MOOSChomp(val,","); //Chomp ystr
        		cout << mysub << endl;
        		MOOSChomp(mysub,"=");
        		cout << mysub << endl;
        		yvec.push_back(boost::lexical_cast<double>(mysub));
        	}
        }
    }
return(true);
}

void GenPath::solveTSP()
{
	//Use the following heuristic to generate a path:
	//Greedy 2-Opt (Guarantee 2-approx of opt)
	//Construct an initial solution greedily (closest point)
	cout << "Constructing Initial Solution" << endl;
    vector<double> xtemp = xvec;
    vector<double> ytemp = yvec;
    double current_obj=0;
    while(!xtemp.empty()){
    	double current_max = -1;
    	double current_x,current_y;
    	int current_ind;
    	for(int i=0;i<xtemp.size();i++){
    		double dist = sqrt(pow(xtemp[i]-myx,2)+pow(ytemp[i]-myy,2));
    		if(current_max<0 || dist<current_max){
    			current_max = dist;
    			current_x = xtemp[i];
    			current_y = ytemp[i];
    			current_ind = i;
    		}
    	}
    	current_obj+=current_max;
    	xsol.push_back(current_x);
    	ysol.push_back(current_y);
    	xtemp.erase(xtemp.begin()+current_ind);
    	ytemp.erase(ytemp.begin()+current_ind);
    }

    publishSegList(xsol,ysol,"initial_tsp");
    cout << "Improving Solution" << endl;

    xtemp = xsol;
    ytemp = ysol;
    //xtemp and ytemp also contain initial greedy solution
    //Improvement step - make pairwise exchanges
    bool improving = true;
    int i = 1;
    int j = i+1;
    double improv_obj = 0;
    while(improving){
    	double tempx = xsol[i]; double tempy = ysol[i];
    	xtemp[i]=xtemp[j]; ytemp[i]=ytemp[j];
    	xtemp[i]=tempx; ytemp[i]=tempy;
    	improv_obj = computeTSPDist(xtemp,ytemp);
    	if(improv_obj>=current_obj){ //no improvement
    		if(j<xvec.size()-1){
    			xtemp = xsol; ytemp = ysol; //reset
    			j=j+1;
    		}else if (j==xvec.size()-1){
    			xtemp = xsol; ytemp = ysol; //reset
    			i=i+1;
    			j=i+1;
    		}else{//Terminate
    			improving = false;	//Finished improving
    		}
    	}
    	else{
    		current_obj = improv_obj; //improve
    		xsol = xtemp; ysol = ytemp;
    		i=1;
    		j=i+1;
    		publishSegList(xsol,ysol,"improved_tsp");
    	}
    }
}

void GenPath::publishSegList(vector<double> xin, vector<double> yin, string label){
	stringstream pointsOut;
    pointsOut <<  "pts={";
	for(int i=0;i<xsol.size();i++){
    	pointsOut << xsol[xsol.size()-i-1] <<","<< ysol[xsol.size()-i-1];
    	if(i<xsol.size()-1){
    		pointsOut << ":";
    	}
    }
	pointsOut << "},label="<<label;
	Notify("VIEW_SEGLIST",pointsOut.str());
}

double GenPath::computeTSPDist(vector<double> xin, vector<double> yin){
	double total_distance;
	for(int i=1;i<xin.size();i++){
		total_distance = sqrt(pow(xin[i-1]-xin[i],2)+pow(yin[i-1]-yin[i],2));
	}
	return total_distance;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
{
    return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenPath::Iterate()
{
    return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
{
    m_MissionReader.GetConfigurationParam("vname", vname);
    MOOSToUpper(vname);
    myvar = "VISIT_POINT_"+vname;
	m_Comms.Register(myvar, 0);
	m_Comms.Notify("TIMER_START","true");

    string start_pos;
    m_MissionReader.GetConfigurationParam("start_pos", start_pos);
    myx = boost::lexical_cast<double>(MOOSChomp(start_pos,","));
    myy = boost::lexical_cast<double>(start_pos);

    return true;
}

