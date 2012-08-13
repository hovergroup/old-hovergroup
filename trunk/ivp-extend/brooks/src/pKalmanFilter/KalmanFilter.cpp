/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: KalmanFilter.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "KalmanFilter.h"
//---------------------------------------------------------
// Constructor

KalmanFilter::KalmanFilter()
{
	GetMatrices("matrices.txt");
	GetWaypoints();
	u_hist = 0;

	x_hist = gsl_matrix_alloc(4,1);
	gsl_matrix_set(x_hist,1,1,0);
	gsl_matrix_set(x_hist,2,1,0);
	gsl_matrix_set(x_hist,4,1,0);

	begin = true;
}

//---------------------------------------------------------
// Destructor

KalmanFilter::~KalmanFilter()
{
	gsl_matrix_free(A);
	gsl_matrix_free(B);
	gsl_matrix_free(B_noise);
	gsl_matrix_free(C);
	gsl_matrix_free(Q);
	gsl_matrix_free(R);
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool KalmanFilter::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      string key = msg.GetKey();

      if(key=="COMPASS_HEADING_FILTERED"){
    	  if(begin){
    		  gsl_matrix_set(x_hist,3,1,0);
    	  }
    	  myheading = msg.GetDouble();
      }
      else if(key=="GPS_X"){
    	  if(begin){
    		  x1 = msg.GetDouble();
    		  x2 = wpx[0];
    	  }
    	  else{
    		  myx = msg.GetDouble();
    	  }
      }
      else if(key=="GPS_Y"){
    	  if(begin){
    		  y1 = msg.GetDouble();
    		  y2 = wpy[0];
    		  GetDesiredHeading();
    		  GetCrossTrackError();
    		  begin = false;
    	  }
    	  else{
    		myy = msg.GetDouble();
    	  }
      }
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool KalmanFilter::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	m_MissionReader.GetConfigurationParam("SegmentTime",segment_time);

	m_Comms.Register("COMPASS_HEADING_FILTERED",0);
	m_Comms.Register("GPS_X",0);
	m_Comms.Register("GPS_Y",0);

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool KalmanFilter::Iterate()
{
   // happens AppTick times per second
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool KalmanFilter::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

void KalmanFilter::GetMatrices(string txtfile){
	FILE* f=fopen(txtfile.c_str(),"r");

	A = gsl_matrix_alloc(4,4);
	B = gsl_matrix_alloc(4,1);
	B_noise = gsl_matrix_alloc(4,4);
	C = gsl_matrix_alloc(2,4);
	Q = gsl_matrix_alloc(4,4);
	R = gsl_matrix_alloc(2,2);

	gsl_matrix_fscanf(f,A);
	cout << "Read A, ";
	gsl_matrix_fscanf(f,B);
	cout << "B, ";
	gsl_matrix_fscanf(f,B_noise);
	cout << "B_noise, ";
	gsl_matrix_fscanf(f,C);
	cout << "C, ";
	gsl_matrix_fscanf(f,Q);
	cout << "Q, ";
	gsl_matrix_fscanf(f,R);
	cout << "R --> " << "End Reading"<< endl;

	fclose(f);
}

void KalmanFilter::GetWaypoints(){ //Waypoints Ordered
	string filename = "kf_waypoints.txt";
	string one_point;
	ifstream waypointsfile("kf_waypoints.txt",ifstream::in);
	int total_points = 0;

	cout<<"Reading Points"<<endl;

	while(waypointsfile.good()){
		getline(waypointsfile,one_point);
		int pos = one_point.find(',');

		if(pos>0){
			total_points++;
			string subx = one_point.substr(0,pos-1);
			wpx.push_back(atof(subx.c_str()));

			std::string suby = one_point.substr(pos+1);
			wpy.push_back(atof(suby.c_str()));

			stringstream ss;
			ss<<"type=gateway,x="<<atof(subx.c_str())<<
					",y="<<atof(suby.c_str())<<",SCALE=4.3,label="<<total_points<<",COLOR=red,width=4.5";
			m_Comms.Notify("VIEW_MARKER",ss.str());
		}
	}

	cout<<"Read "<<total_points<<" points."<<std::endl;
}
