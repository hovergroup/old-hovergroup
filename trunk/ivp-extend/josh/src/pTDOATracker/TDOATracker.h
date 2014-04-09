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
   void GetPriors();
   void TempUpdate();
   void FullUpdate();
   void NotifyStatus(int,std::vector<int>, std::string);
   gsl_matrix* MatrixSquareRoot(int, gsl_matrix*);

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   std::vector<gsl_matrix*> s1,s2,s3,u1,u2,u3;
   std::vector<gsl_matrix*> error_cov;
   gsl_matrix *P;
   gsl_vector *w, *xhat;
   double vol, Q, R, localNoise, dt;
   int s_dim;
   TDOAUpdate protobuf;
   std::vector<TDOAData> messages;
   std::vector<int> slots_heard;
   boost::variate_generator<boost::mt19937, boost::normal_distribution<> > generator;
   std::string msg_out, state;

 private: // Configuration variables
   int  tdoa_id;

 private: // State variables
   double navx,navy;

};

#endif 
