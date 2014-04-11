/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOATracker.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "TDOATracker.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TDOATracker::TDOATracker() : generator(boost::mt19937(time(0)),boost::normal_distribution<>())
{
	s_dim = 3;vol = 0;
	navx = 0;navy = 0;
	tdoa_id = 9;
	dt = 5;
	msg_out = "initialized";
	state = "initial"; state_num = 4;
	slots_heard = vector<int>(3,0);
	xhat = gsl_vector_alloc(3);
	gsl_vector_set_zero(xhat);
	P = gsl_matrix_alloc(3,3);
	gsl_matrix_set_identity(P);
	temp_control = 0;
	my_label="default";my_color="orange";
}

//---------------------------------------------------------
// Destructor

TDOATracker::~TDOATracker()
{
	for(int i=0;i<s_dim;i++){
		gsl_matrix_free(s1[i]);
		gsl_matrix_free(s2[i]);
		gsl_matrix_free(s3[i]);
		gsl_matrix_free(u1[i]);
		gsl_matrix_free(u2[i]);
		gsl_matrix_free(u3[i]);
	}
	gsl_matrix_free(P);
	gsl_vector_free(w);
	gsl_vector_free(xhat);
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TDOATracker::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();

		if(key== "TDOA_PROTOBUF"){
			protobuf.ParseFromString(msg.GetString());
			slots_heard = vector<int>(3,0);
			int data_heard = protobuf.data_size();
			state_num = protobuf.cycle_state();

			switch (protobuf.cycle_state()){
			case 0:
				state = "initial"; //Start of new cycle
				msg_out = "New Cycle";
				//With just the range ping, do nothing

				break;
			case 1: case 2: case 3:
				if(data_heard>0){	//Fill data
					messages = vector<TDOAData>(data_heard,TDOAData());
					for(int i=0;i<data_heard;i++){
						TDOAData temp = protobuf.data(i);
						slots_heard[temp.id()-1] = 1;
						messages[i] = protobuf.data(i);
					}
				}

				msg_out = "Not enough data!";

				if(state!="full"){	//Try to make a FULL update
					if(data_heard==3){	//Make a Full update
						FullUpdate();
						msg_out = "Full Update!";
						state = "full";

						std::stringstream ss;
						ss << gsl_vector_get(xhat,1) << "," << gsl_vector_get(xhat,2);
						m_Comms.Notify("TDOA_FULL_ESTIMATE", ss.str());
						cout <<"Full: "<< ss.str() << endl;
						DrawTarget(gsl_vector_get(xhat,1),gsl_vector_get(xhat,2),"Full");
					}
				}

				if(state=="initial"){	//Try to make TEMP update
					if(data_heard==2){ //Make a Temp Update
						TempUpdate();
						msg_out = "Temp Update";
						state="temp";

						std::stringstream ss;
						ss << gsl_vector_get(xhat,1) << "," << gsl_vector_get(xhat,2);
						m_Comms.Notify("TDOA_TEMP_ESTIMATE", ss.str());
						cout <<"Temp: " << ss.str() << endl;
						DrawTarget(gsl_vector_get(xhat,1),gsl_vector_get(xhat,2),"Temp");

					}
				}

				break;
			case 4:
				msg_out = "Paused";
				state = "pause";
				break;
			}

			NotifyStatus(protobuf.cycle_state(),slots_heard,msg_out);

		}

		// update nav info
		else if (key == "NAV_X") {
			navx = msg.GetDouble();
		} else if (key == "NAV_Y") {
			navy = msg.GetDouble();
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TDOATracker::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	m_MissionReader.GetConfigurationParam("TDOAid",tdoa_id);
	m_MissionReader.GetConfigurationParam("Q",Q);
	m_MissionReader.GetConfigurationParam("R",R);
	m_MissionReader.GetConfigurationParam("SDim",s_dim);	//number of sigma points
	m_MissionReader.GetConfigurationParam("ODEdt",dt);	//ODE system timestep
	m_MissionReader.GetConfigurationParam("xRel",x_rel);
	m_MissionReader.GetConfigurationParam("yRel",y_rel);
	m_MissionReader.GetConfigurationParam("TempControl",temp_control);
	m_MissionReader.GetConfigurationParam("Label",my_label);
	m_MissionReader.GetConfigurationParam("Color",my_color);

	// Getting Sigma Points
	string txtfile = "HermiteMatrices.txt";
	s1 = vector<gsl_matrix*>(s_dim);
	s2 = vector<gsl_matrix*>(s_dim);
	s3 = vector<gsl_matrix*>(s_dim);
	u1 = vector<gsl_matrix*>(s_dim);
	u2 = vector<gsl_matrix*>(s_dim);
	u3 = vector<gsl_matrix*>(s_dim);

	FILE* f = fopen(txtfile.c_str(),"r");
	cout << "Reading Sigma Point Matrices\n";

	for(int i=0;i<s_dim;i++){
		s1[i] = gsl_matrix_alloc(s_dim, s_dim);
		gsl_matrix_fscanf(f,s1[i]);
	}

	for(int i=0;i<s_dim;i++){
		s2[i] = gsl_matrix_alloc(s_dim, s_dim);
		gsl_matrix_fscanf(f,s2[i]);
	}

	for(int i=0;i<s_dim;i++){
		s3[i] = gsl_matrix_alloc(s_dim, s_dim);
		gsl_matrix_fscanf(f,s3[i]);
	}

	for(int i=0;i<s_dim;i++){
		u1[i] = gsl_matrix_alloc(s_dim, s_dim);
		u2[i] = gsl_matrix_alloc(s_dim, s_dim);
		u3[i] = gsl_matrix_alloc(s_dim, s_dim);
	}
	w = gsl_vector_alloc(s_dim);
	gsl_vector_fscanf(f,w);
	fscanf (f, "%lf", &vol);
	cout << "Read All\n";
	fclose(f);

	RegisterVariables();

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOATracker::Iterate()
{
	return(true);
}

void TDOATracker::TempUpdate(){
	cout << "Temp Update" << endl;
	vector<double> x (2,0),y (2,0),toa (2,0),r (2,0);
	for(int i=0;i<2;i++){
		x[i] = messages[i].x();
		y[i] = messages[i].y();
		toa[i] = messages[i].toa();
		r[i] = sqrt(pow(gsl_vector_get(xhat,1)-x[i],2)+pow(gsl_vector_get(xhat,2)-y[i],2));
	}

	//Check wraparound
	if(abs(toa[0]-toa[1])>0.5){
		cout << "Wraparound Occurred" << endl;
		if(toa[1]<0.5)
			toa[1]+=1;
		else
			toa[0]+=1;
	}

	double z = (toa[0]-toa[1])*1492; //tdoa -- actual observation
	double zhat = r[0]-r[1];	//estimated observation

	gsl_vector *Hk = gsl_vector_alloc(3);
	gsl_vector_set(Hk,0,0);
	double temp = 1/r[0]*(gsl_vector_get(xhat,1)-x[0])-1/r[1]*(gsl_vector_get(xhat,1)-x[1]);
	gsl_vector_set(Hk,1,temp);

	temp = 1/r[0]*(gsl_vector_get(xhat,2)-y[0])-1/r[1]*(gsl_vector_get(xhat,2)-y[1]);
	gsl_vector_set(Hk,2,temp);

	gsl_vector* tvec = gsl_vector_alloc(3);
	gsl_blas_dgemv(CblasNoTrans,1,P,Hk,0,tvec); //P*Hk'
	double sk = 0;
	gsl_blas_ddot(Hk,tvec,&sk);//Hk*P*Hk'
	gsl_vector_scale(tvec,1/(sk+R));				//tvec = Kk
	gsl_vector_scale(tvec,(z-zhat));
	gsl_vector_add(xhat,tvec);

	gsl_matrix* Po = gsl_matrix_alloc(3,3);
	gsl_matrix* ident = gsl_matrix_alloc(3,3);
	gsl_matrix_set_identity(ident);

	//outer product Kk*Hk
	for(int l=0;l<3;l++){
		gsl_vector_view column = gsl_matrix_column(Po,l);
		gsl_vector_set(&column.vector,0,gsl_vector_get(tvec,0));
		gsl_vector_set(&column.vector,1,gsl_vector_get(tvec,1));
		gsl_vector_set(&column.vector,2,gsl_vector_get(tvec,2));

		gsl_vector_scale(&column.vector,gsl_vector_get(Hk,l));
	}

	gsl_matrix_sub(ident,Po);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,ident,P,0,Po);
	gsl_matrix_memcpy(P,Po);

	std::stringstream ss;
	ss << gsl_vector_get(xhat,1) << "," << gsl_vector_get(xhat,2);
	cout <<"X with z: " << ss.str() << endl;

//	for(int i=0;i<3;i++){
//		cout << gsl_matrix_get(P,i,0) << ","<< gsl_matrix_get(P,i,1) << ","<< gsl_matrix_get(P,i,2) << endl;
//	}

	//Get Priors, number of delay states
	for(int i=0;i<state_num;i++){
		GetPriors();
		std::stringstream ss;
		ss << gsl_vector_get(xhat,1) << "," << gsl_vector_get(xhat,2);
		cout <<"X prior: " << ss.str() << endl;
	}

	gsl_vector_free(Hk);
	gsl_vector_free(tvec);
	gsl_matrix_free(Po);
	gsl_matrix_free(ident);

	if(temp_control==1){
		double x_control = x_rel + gsl_vector_get(xhat,1);
		double y_control = y_rel + gsl_vector_get(xhat,2);
		//Control Action
		std::stringstream ss;
		ss << "points=" << x_control << "," << y_control;
		m_Comms.Notify("TDOA_WAYPOINT_UPDATES", ss.str());
		m_Comms.Notify("TDOA_STATION", "false");
	}
}

void TDOATracker::FullUpdate(){
	cout << "Full Update" << endl;
	vector<double> x (3,0),y (3,0),toa (3,0),r (3,0);
	for(int i=0;i<3;i++){
		x[i] = messages[i].x();
		y[i] = messages[i].y();
		toa[i] = messages[i].toa();
		r[i] = sqrt(pow(gsl_vector_get(xhat,1)-x[i],2)+pow(gsl_vector_get(xhat,2)-y[i],2));
	}

	//Check for wrap around
	int wraparound = 0;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(i!=j){
				if(abs(toa[i]-toa[j])>0.5){
					wraparound = 1;
				}
			}
		}
	}
	if(wraparound==1){
		cout << "Wraparound Occurred" << endl;
		for(int i=0;i<3;i++){
			if(toa[i]<0.5)
				toa[i]+=1;
		}
	}

	gsl_vector *z = gsl_vector_alloc(2);
	gsl_vector_set(z,0,(toa[0]-toa[1])*1492);
	gsl_vector_set(z,1,(toa[1]-toa[2])*1492);

	gsl_vector *zhat = gsl_vector_alloc(2);
	gsl_vector_set(zhat,0,r[0]-r[1]);
	gsl_vector_set(zhat,1,r[1]-r[2]);

	gsl_matrix *Hk = gsl_matrix_alloc(2,3);
	gsl_matrix_set(Hk,0,0,0);
	gsl_matrix_set(Hk,1,0,0);
	double temp = 1/2/r[0]*2*(gsl_vector_get(xhat,1)-x[0])-1/2/r[1]*2*(gsl_vector_get(xhat,1)-x[1]);
	gsl_matrix_set(Hk,0,1,temp);
	temp = 1/2/r[0]*2*(gsl_vector_get(xhat,2)-y[0])-1/2/r[1]*2*(gsl_vector_get(xhat,2)-y[1]);
	gsl_matrix_set(Hk,0,2,temp);

	temp = 1/2/r[1]*2*(gsl_vector_get(xhat,1)-x[1])-1/2/r[2]*2*(gsl_vector_get(xhat,1)-x[2]);
	gsl_matrix_set(Hk,1,1,temp);
	temp = 1/2/r[1]*2*(gsl_vector_get(xhat,2)-y[1])-1/2/r[2]*2*(gsl_vector_get(xhat,2)-y[2]);
	gsl_matrix_set(Hk,1,2,temp);

	gsl_matrix* tmat = gsl_matrix_alloc(3,2);
	gsl_matrix* Kk = gsl_matrix_alloc(3,2);
	gsl_matrix* hph = gsl_matrix_alloc(2,2);
	gsl_matrix* Rmat = gsl_matrix_alloc(2,2);
	gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,P,Hk,0,tmat); //P*Hk'
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,Hk,tmat,0,hph); //Hk*P*Hk'
	gsl_matrix_set(Rmat,0,0,R);
	gsl_matrix_set(Rmat,1,1,R); //[R 0;0 R];
	gsl_matrix_add(hph,Rmat); // HkPHk + R

	gsl_permutation * perm = gsl_permutation_alloc(2);
	int s;
	gsl_linalg_LU_decomp (hph, perm, &s);
	gsl_linalg_LU_invert (hph, perm, Rmat); //Rmat contains inv(HPH+R)

	gsl_blas_dgemm(CblasTrans,CblasNoTrans,1,Hk,Rmat,0,tmat);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,P,tmat,0,Kk); //Kk contains K

	gsl_vector_sub(z,zhat); //stored in z
	gsl_vector* txhat = gsl_vector_alloc(3);
	gsl_blas_dgemv(CblasNoTrans,1,Kk,z,0,txhat);
	gsl_vector_add(xhat,txhat);

	gsl_matrix* Po = gsl_matrix_alloc(3,3);
	gsl_matrix* ident = gsl_matrix_alloc(3,3);
	gsl_matrix_set_identity(ident);

	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,Kk,Hk,0,Po);
	gsl_matrix_sub(ident,Po);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,ident,P,0,P);

	//Get Priors, number of delay states
	for(int i=0;i<state_num;i++){
		GetPriors();
	}

	gsl_matrix_free(Hk);
	gsl_matrix_free(Kk);
	gsl_matrix_free(tmat);
	gsl_matrix_free(hph);
	gsl_matrix_free(Rmat);
	gsl_matrix_free(Po);
	gsl_matrix_free(ident);
	gsl_vector_free(txhat);

	double x_control = x_rel + gsl_vector_get(xhat,1);
	double y_control = y_rel + gsl_vector_get(xhat,2);
	//Control Action
	std::stringstream ss;
	ss << "points=" << x_control << "," << y_control;
	m_Comms.Notify("TDOA_WAYPOINT_UPDATES", ss.str());
	m_Comms.Notify("TDOA_STATION", "false");
}

void TDOATracker::GetPriors(){

	gsl_vector *target = gsl_vector_alloc(3);
	gsl_vector *dum = gsl_vector_alloc(3);
	gsl_matrix *Po = gsl_matrix_alloc(3,3);
	gsl_matrix *sP = gsl_matrix_alloc(s_dim,s_dim);

	MatrixSquareRoot(s_dim,P,sP);

	for(int i=0;i<s_dim;i++){	//iterating over sigma points
		for(int j=0;j<s_dim;j++){
			for(int k=0;k<s_dim;k++){

				gsl_vector_set(target,0,gsl_matrix_get(s1[i],j,k));
				gsl_vector_set(target,1,gsl_matrix_get(s2[i],j,k));
				gsl_vector_set(target,2,gsl_matrix_get(s3[i],j,k));

				cout << gsl_vector_get(target,0) << "," << gsl_vector_get(target,1) <<  "," << gsl_vector_get(target,2) << endl ;

				gsl_blas_dgemv(CblasNoTrans,1,sP,target,0,dum);
				gsl_vector_add(dum,xhat);

				localNoise = sqrt(Q)*generator();

				//cout<< "yin: " << gsl_vector_get(dum,0) << "," << gsl_vector_get(dum,1) <<  "," << gsl_vector_get(dum,2) << endl ;

				gsl_odeiv2_system sys = {func, jac, 3, &localNoise};

				gsl_odeiv2_driver * d = gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rkf45,1e-6, 1e-6, 0.0);
				double t = 0.0;
				double y[] = {gsl_vector_get(dum,0),gsl_vector_get(dum,1),gsl_vector_get(dum,2)};
				int status = gsl_odeiv2_driver_apply (d, &t, dt, y);

				if (status != GSL_SUCCESS){
					printf ("GSL ODE error, return value=%d\n", status);
					break;
				}

				gsl_matrix_set(u1[i],j,k,y[0]);
				gsl_matrix_set(u2[i],j,k,y[1]);
				gsl_matrix_set(u3[i],j,k,y[2]);

				//cout << "yout: "<< y[0] << "," << y[1] <<  "," << y[2] << endl ;

				gsl_odeiv2_driver_free (d);
			}
		}
	}

	gsl_vector_set_zero(xhat);
	//get quadrature mean
	for(int i=0;i<s_dim;i++){
		for(int j=0;j<s_dim;j++){
			for(int k=0;k<s_dim;k++){
				gsl_vector_set(target,0,gsl_matrix_get(u1[i],j,k));
				gsl_vector_set(target,1,gsl_matrix_get(u2[i],j,k));
				gsl_vector_set(target,2,gsl_matrix_get(u3[i],j,k));
				double wfactor = gsl_vector_get(w,i)*gsl_vector_get(w,j)*gsl_vector_get(w,k);
				gsl_vector_scale(target,wfactor);
				gsl_vector_add(xhat,target);
			}
		}
	}
	gsl_vector_scale(xhat,1/vol);

	gsl_matrix_set_zero(P);
	//get quadrature variance
	for(int i=0;i<s_dim;i++){
		for(int j=0;j<s_dim;j++){
			for(int k=0;k<s_dim;k++){
				gsl_vector_set(target,0,gsl_matrix_get(u1[i],j,k));
				gsl_vector_set(target,1,gsl_matrix_get(u2[i],j,k));
				gsl_vector_set(target,2,gsl_matrix_get(u3[i],j,k));
				gsl_vector_sub(target,xhat);

				//outer product
				for(int l=0;l<3;l++){
					gsl_vector_view column = gsl_matrix_column(Po,l);
					gsl_vector_set(&column.vector,0,gsl_vector_get(target,0));
					gsl_vector_set(&column.vector,1,gsl_vector_get(target,1));
					gsl_vector_set(&column.vector,2,gsl_vector_get(target,2));

					gsl_vector_scale(&column.vector,gsl_vector_get(target,l));
				}

				double wfactor = gsl_vector_get(w,i)*gsl_vector_get(w,j)*gsl_vector_get(w,k);
				gsl_matrix_scale(Po,wfactor);
				gsl_matrix_add(P,Po);
			}
		}
	}
	gsl_matrix_scale(P,1/vol);
	gsl_vector_free(target);
	gsl_vector_free(dum);
	gsl_matrix_free(Po);
	gsl_matrix_free(sP);
}

int func(double t, const double y[], double f[], void *params){
	f[0] = *(double *)params;
	f[1] = 0.1*cos(y[0]);
	f[2] = 0.1*sin(y[0]);
	return GSL_SUCCESS;
}

int jac(double t, const double y[], double *dfdy, double dfdt[], void *params){
	double mu = *(double *)params;
	gsl_matrix_view dfdy_mat = gsl_matrix_view_array (dfdy, 3, 3);
	gsl_matrix * m = &dfdy_mat.matrix;
	gsl_matrix_set_zero(m);
	dfdt[0] = 0.0;
	dfdt[1] = 0.0;
	return GSL_SUCCESS;
}

void TDOATracker::MatrixSquareRoot(int dim, gsl_matrix * matrix_in, gsl_matrix * matrix_out){
	gsl_permutation * perm = gsl_permutation_alloc(dim);
	gsl_matrix * Ptemp = gsl_matrix_alloc(dim,dim);
	int s;
	gsl_vector_complex *eval = gsl_vector_complex_alloc(dim);
	gsl_matrix_complex *evec = gsl_matrix_complex_alloc(dim, dim);

	gsl_eigen_nonsymmv_workspace *w = gsl_eigen_nonsymmv_alloc(dim);

	gsl_eigen_nonsymmv(matrix_in, eval, evec, w);
	gsl_eigen_nonsymmv_free(w);
	gsl_eigen_nonsymmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_DESC);

	gsl_vector_view eval_view = gsl_vector_complex_real(eval);
	gsl_vector *eval_real = &eval_view.vector;

//	for(int i=0;i<dim;i++){
//		cout<<gsl_vector_get(eval_real,i)<<endl;
//	}

	gsl_matrix *evec_real = gsl_matrix_alloc(dim,dim);

	for (int i = 0; i < dim; i++){
		for (int j = 0; j < dim; j++){
			gsl_matrix_set(evec_real,i,j,GSL_REAL(gsl_matrix_complex_get(evec, i, j)));
		}
	}

//	for(int m=0;m<3;m++){
//		cout << gsl_matrix_get(evec_real,m,0) << ","<< gsl_matrix_get(evec_real,m,1) << ","<< gsl_matrix_get(evec_real,m,2) << endl;
//	}

	gsl_matrix *sqrt_e = gsl_matrix_alloc(dim,dim);
	gsl_matrix_set_zero(sqrt_e);
	for(int i=0;i<dim;i++){
		gsl_matrix_set(sqrt_e,i,i,sqrt(gsl_vector_get(eval_real,i)));
	}

//	for(int m=0;m<3;m++){
//		cout << gsl_matrix_get(sqrt_e,m,0) << ","<< gsl_matrix_get(sqrt_e,m,1) << ","<< gsl_matrix_get(sqrt_e,m,2) << endl;
//	}

	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,evec_real,sqrt_e,0.0,Ptemp);

//	for(int m=0;m<3;m++){
//		cout << gsl_matrix_get(Ptemp,m,0) << ","<< gsl_matrix_get(Ptemp,m,1) << ","<< gsl_matrix_get(Ptemp,m,2) << endl;
//	}

	gsl_matrix * evec_inv = gsl_matrix_alloc(dim,dim);
	gsl_linalg_LU_decomp (evec_real, perm, &s);
	gsl_linalg_LU_invert (evec_real, perm, evec_inv);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,Ptemp,evec_inv,0.0,matrix_out);

//	for(int m=0;m<3;m++){
//		cout << gsl_matrix_get(matrix_out,m,0) << ","<< gsl_matrix_get(matrix_out,m,1) << ","<< gsl_matrix_get(matrix_out,m,2) << endl;
//	}

	gsl_vector_complex_free(eval);
	gsl_matrix_complex_free(evec);
	gsl_permutation_free(perm);
	gsl_matrix_free(sqrt_e);
	gsl_matrix_free(evec_real);
	gsl_matrix_free(evec_inv);
	gsl_matrix_free(Ptemp);

}

void TDOATracker::DrawTarget( double x, double y, string my_type ) {
	std::stringstream ss;
	ss << "type=" << "diamond";
	ss << ",x=" << x;
	ss << ",y=" << y;
	ss << ",label=" << my_label;
	ss << ",COLOR=" << my_color;
	ss << ",msg=" << my_label<<","<< my_type << ": " << (int) x << " " << (int) y;

	m_Comms.Notify("VIEW_MARKER", ss.str() );
}

void TDOATracker::NotifyStatus(int cycle_id, vector<int> message_ids, string msg_out){
	stringstream tellme;
	tellme.str("Cycle: ");
	tellme << cycle_id << ' ';
	tellme << "Heard: ";
	for (vector<int>::iterator it = message_ids.begin() ; it != message_ids.end(); ++it){
		tellme << *it << ' ';
	}
	tellme<<"Message: "<<msg_out;
	m_Comms.Notify("TRACKER_STATUS",tellme.str());
	cout << tellme.str() << endl;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TDOATracker::OnStartUp()
{
	return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void TDOATracker::RegisterVariables() {
	m_Comms.Register("NAV_X", 0);
	m_Comms.Register("NAV_Y", 0);
	m_Comms.Register("TDOA_PROTOBUF", 0);

}
