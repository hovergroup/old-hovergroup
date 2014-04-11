/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOATracker.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TDOATracker_HEADER
#define TDOATracker_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <math.h>
#include "tdoa.pb.h"
#include <vector>
#include <sstream>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

int jac(double, const double[], double*, double[], void*);
int func(double, const double[], double[], void*);

class TDOATracker : public CMOOSApp
{
public:
	TDOATracker();
	~TDOATracker();

	void RegisterVariables();
	void GetPriors(gsl_matrix*,gsl_vector*);
	void TempUpdate();
	void FullUpdate();
	void NotifyStatus(int,std::vector<int>, std::string);
	void MatrixSquareRoot(int, gsl_matrix*, gsl_matrix*);
	void DrawTarget(double, double, std::string);

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

    struct ode_params{
	 double n;
	 double v;
    };

	std::vector<gsl_matrix*> s1,s2,s3,u1,u2,u3;
	gsl_matrix *P, *Ptemp;
	gsl_vector *w, *xhat, *xhat_temp;

	double vol, Q, R, localNoise, dt, target_speed;
	int s_dim, state_num, temp_control;
	TDOAUpdate protobuf;
	std::vector<TDOAData> messages;
	std::vector<int> slots_heard;
	boost::variate_generator<boost::mt19937, boost::normal_distribution<> > generator;
	std::string msg_out, state;
	double x_rel,y_rel;	//rel to target at origin

private: // Configuration variables
	int  tdoa_id;
	std::string my_label, my_color;

private: // State variables
	double navx,navy;

};

#endif 
