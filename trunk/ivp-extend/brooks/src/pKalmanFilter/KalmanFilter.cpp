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
	//Inputs
	GetMatrices("matrices.txt");
	GetWaypoints("waypoints.txt");

	//Sensor
	z = gsl_vector_alloc(2);
	x_des = gsl_vector_calloc(4);		//allocated all 0s

	//Predicted
	x_pre = gsl_vector_alloc(4);
	P_pre = gsl_matrix_alloc(4,4);

	//Computed
	K = gsl_matrix_alloc(4,2);
	x_hat = gsl_vector_calloc(4);	//allocated all 0s
	P = gsl_matrix_calloc(4,4);	//allocated all 0s

	//History
	x_hist = gsl_vector_calloc(4);	//allocated all 0s
	P_hist = gsl_matrix_calloc(4,4);	//allocated all 0s

	u = 0;
	u_hist = 0;

	begin = true;
	end = false;
	wait = 5;
	offset = 0;
	wp_id = 0;
}

//---------------------------------------------------------
// Destructor

KalmanFilter::~KalmanFilter()
{
	gsl_matrix_free(A);
	gsl_matrix_free(B);
	gsl_matrix_free(B_noise);
	gsl_matrix_free(B_in);
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
			myheading = msg.GetDouble();
		}
		else if(key=="GPS_X"){
			myx = msg.GetDouble();
		}
		else if(key=="GPS_Y"){
			myy = msg.GetDouble();
			if(begin){}
			else{EstimateStates();}
		}
		else if(key=="DESIRED_RUDDER"){
			u_hist = u;
			u = msg.GetDouble();
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

	m_MissionReader.GetConfigurationParam("Speed",speed);
	m_MissionReader.GetConfigurationParam("Thrust",thrust);

	m_Comms.Register("COMPASS_HEADING_FILTERED",0);
	m_Comms.Register("GPS_X",0);
	m_Comms.Register("GPS_Y",0);
	m_Comms.Register("DESIRED_RUDDER",0);

	start_time = MOOSTime();
	m_Comms.Notify("MOOS_MANUAL_OVERRIDE","true");

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool KalmanFilter::Iterate()
{
	// happens AppTick times per second

	double time_passed = MOOSTime() - start_time;

	if(!end){
		if(time_passed >= wait){
			if(begin){	//---------Initialize

				cout << "Initializing" << endl;
				m_Comms.Notify("DESIRED_THRUST",thrust);
				m_Comms.Notify("DESIRED_RUDDER",0);
				m_Comms.Notify("MOOS_MANUAL_OVERRIDE","false");

				x1 = myx;
				y1 = myy;
				x2 = wpx[wp_id];
				y2 = wpy[wp_id];

				wait = GetDistance(x1,y1,x2,y2)/speed;
				offset = wait;
				start_time = MOOSTime();

				cout << "Sending MPC Command" << endl;
				m_Comms.Notify("MPC_STOP","GO");
				begin = false;
			}
			else if(wp_id==wpx.size()-1){ //-------------End
				cout << "Ending Mission" << endl;
				m_Comms.Notify("MOOS_MANUAL_OVERRIDE","true");
				m_Comms.Notify("DESIRED_THRUST",0);
				m_Comms.Notify("DESIRED_RUDDER",0);
				end = true;
			}
			else{
				cout << "Reached Turn" << endl;
				wp_id++;
				wait = time[wp_id] + offset;
				x1 = x2;
				y1 = y2;
				x2 = wpx[wp_id];
				y2 = wpy[wp_id];
			}

			cout << "Going to: " << wpx[wp_id] << " , " << wpy[wp_id] << endl;
			cout << "Expected time to reach: " << wait << endl;
		}

		cout << "Time left: " << (wait-time_passed) << endl;
		cout << "Distance left: " << GetDistance(myx,myy,x2,y2) << endl;
	}

	else{
		cout << "Mission Ended" << endl;
	}
	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool KalmanFilter::OnStartUp()
{
	// happens before connection is open

	return(true);
}

void KalmanFilter::EstimateStates(){
	UpdateSensorReadings();

	gsl_vector_set(x_des,3,headings[wp_id]);
	gsl_vector_memcpy(x_hist,x_hat);
	gsl_matrix_memcpy(P_hist,P);

	gsl_matrix *temp_matrix, *temp_matrix_2;
	gsl_vector *temp_vector;

	temp_matrix = gsl_matrix_calloc(4,1);
	temp_vector = gsl_vector_calloc(4);
	gsl_blas_dgemv(CblasNoTrans,1.0, A, x_hist,0.0, x_pre);
	gsl_matrix_memcpy(temp_matrix,B);
	gsl_matrix_scale(temp_matrix, u_hist);
	gsl_matrix_get_col(temp_vector,temp_matrix,1);
	gsl_vector_add(x_pre,temp_vector);
	gsl_blas_dgemv(CblasNoTrans,1.0, B_in, x_des,0.0, temp_vector);
	gsl_vector_add(x_pre,temp_vector);
	gsl_matrix_free(temp_matrix);
	gsl_vector_free(temp_vector);
	cout << "Got x_pre: " << endl;
	//gsl_vector_fprintf(stdout,x_pre,"%f");
	//cout << endl;

	temp_matrix = gsl_matrix_calloc(4,4);
	temp_matrix_2 = gsl_matrix_calloc(4,4);
	gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,P_hist,A,0.0,temp_matrix);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,A,temp_matrix,0.0,P_pre);
	gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,Q,B_noise,0.0,temp_matrix);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,B_noise,temp_matrix,0.0,temp_matrix_2);
	gsl_matrix_add(P_pre,temp_matrix_2);
	cout << "Got P_pre: " << endl;
	//gsl_matrix_fprintf(stdout,P_pre,"%f");
	//cout << endl;
	gsl_matrix_free(temp_matrix);
	gsl_matrix_free(temp_matrix_2);

	temp_matrix = gsl_matrix_calloc(2,2);
	gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,P_pre,C,0.0,temp_matrix);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,C,temp_matrix,0.0,K);
	gsl_matrix_add(K,R);
	int s;
	gsl_permutation * p = gsl_permutation_alloc (4);
	gsl_matrix_memcpy(temp_matrix,K);
	gsl_linalg_LU_decomp (temp_matrix, p, &s);
	gsl_linalg_LU_invert(temp_matrix,p,K);
	gsl_blas_dgemm(CblasTrans,CblasNoTrans,1.0,C,K,0.0,temp_matrix);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,P_pre,temp_matrix,0.0,K);
	cout << "Got K: " << endl;
	//gsl_matrix_fprintf(stdout,K,"%f");
	//cout << endl;
	gsl_matrix_free(temp_matrix);
	gsl_permutation_free(p);

	temp_vector = gsl_vector_calloc(2);
	gsl_blas_dgemv(CblasNoTrans,1.0, C, x_hist,0.0, x_hat);
	gsl_vector_memcpy(temp_vector,z);
	gsl_vector_sub(temp_vector,x_hat);
	gsl_vector_add(x_hat,x_pre);
	cout << "Got X_hat: " << endl;
	//gsl_vector_fprintf(stdout,x_hat,"%f");
	//cout << endl;
	gsl_vector_free(temp_vector);

	temp_matrix = gsl_matrix_alloc(4,4);
	temp_matrix_2 = gsl_matrix_alloc(4,4);
	gsl_matrix_set_identity(temp_matrix);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,K,C,0.0,temp_matrix_2);
	gsl_matrix_sub(temp_matrix,temp_matrix_2);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,temp_matrix,P_pre,0.0,P);
	cout << "Got P: " << endl;
	//gsl_matrix_fprintf(stdout,P,"%f");
	//cout << endl;
	gsl_matrix_free(temp_matrix);
	gsl_matrix_free(temp_matrix_2);

	PublishStates();
}

void KalmanFilter::UpdateSensorReadings(){
	cout << "Updating Sensor Readings" << endl;
	gsl_vector_set(z,1,GetCrossTrackError());
	gsl_vector_set(z,2,myheading);
	gsl_vector_fprintf(stdout,z,"%f");
}

double KalmanFilter::GetCrossTrackError(){
	cout << "Getting Cross Track Error" << endl;
	double ct_error;
	double a = GetDistance(myx,myy,x1,y1);
	double b = GetDistance(myx,myy,x2,y2);
	double c = GetDistance(x1,y1,x2,y2);
	double cos_a = (pow(a,2.0) + pow(c,2.0) - pow(b,2.0))/(2*a*c);
	double d = a*cos_a;
	ct_error = sqrt(pow(a,2.0)-pow(d,2.0));
	cout << "Calculated Cross Track Error: " << ct_error << endl;
	return ct_error;
}

double KalmanFilter::GetDesiredHeading(){
	double a = x2-x1;
	double b = y2-y1;

	double desired_heading = atan (a/b) * 180 / PI;

	if(a>=0){desired_heading = 90-desired_heading;}
	else{desired_heading = 270-desired_heading;}

	cout << "Calculated Desired Heading: " << desired_heading << endl;
	m_Comms.Notify("DESIRED_HEADING",desired_heading);
	return desired_heading;
}

void KalmanFilter::PublishStates(){
	cout << "Publishing states" << endl;
	string delim = "<|>";
	stringstream ss;
	ss << gsl_vector_get(x_hat,1) << delim;
	ss << gsl_vector_get(x_hat,2) << delim;
	ss << gsl_vector_get(x_hat,3) << delim;
	ss << gsl_vector_get(x_hat,4);
	m_Comms.Notify("MPC_XEST",ss.str());
}

void KalmanFilter::GetMatrices(string txtfile){
	FILE* f=fopen(txtfile.c_str(),"r");

	A = gsl_matrix_alloc(4,4);
	B = gsl_matrix_alloc(4,1);
	B_noise = gsl_matrix_alloc(4,4);
	B_in = gsl_matrix_alloc(4,4);
	C = gsl_matrix_alloc(2,4);
	Q = gsl_matrix_alloc(4,4);
	R = gsl_matrix_alloc(2,2);

	gsl_matrix_fscanf(f,A);
	cout << "Read A, ";
	gsl_matrix_fscanf(f,B);
	cout << "B, ";
	gsl_matrix_fscanf(f,B_noise);
	cout << "B_noise, ";
	gsl_matrix_fscanf(f,B_in);
	cout << "B_in, ";
	gsl_matrix_fscanf(f,C);
	cout << "C, ";
	gsl_matrix_fscanf(f,Q);
	cout << "Q, ";
	gsl_matrix_fscanf(f,R);
	cout << "R --> " << "End Reading"<< endl;

	fclose(f);
}

void KalmanFilter::GetWaypoints(string txtfile){ //Waypoints Ordered
	string one_point;
	ifstream waypointsfile(txtfile.c_str(),ifstream::in);
	int total_points = 0;

	cout<<"Reading Points"<<endl;
	//time, desired heading,x1,y1,x2,y2

	while(waypointsfile.good()){
		getline(waypointsfile,one_point);
		int pos = one_point.find(',');

		if(pos>0){

			stringstream ss;
			char discard;
			double param;

			ss.str(one_point);
			ss >> param; time.push_back(param); ss >> discard;
			ss >> param; headings.push_back(param); ss >> discard;
			ss >> param; wpx.push_back(param); ss >> discard;
			ss >> param; wpy.push_back(param); ss >> discard;

			ss.str("");
			ss<<"type=gateway,x="<<wpx[total_points]<<
					",y="<<wpx[total_points]<<",SCALE=4.3,label="<<total_points<<",COLOR=red,width=4.5";
			m_Comms.Notify("VIEW_MARKER",ss.str());

			total_points++;
		}
	}

	cout<<"Read "<<total_points<<" points."<<std::endl;
}


double KalmanFilter::GetDistance(double xi1,double yi1,double xi2,double yi2){
	return sqrt(pow((xi1-xi2),2.0) + pow((yi1-yi2),2.0));
}
