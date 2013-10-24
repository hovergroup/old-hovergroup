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

class TDOATracker : public CMOOSApp
{
 public:
   TDOATracker();
   ~TDOATracker();

   void GetHermite(std::string);
   void GetPriors(gsl_vector*, gsl_matrix*);
   void TempUpdate(gsl_vector*, gsl_matrix*);
   void FullUpdate(gsl_vector*, gsl_matrix*);
   void NotifyStatus(int,std::vector<int>);
   gsl_matrix* MatrixSquareRoot(int, gsl_matrix*);

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   gsl_matrix *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9;
   gsl_vector *w;
   double vol;
   int s_dim;
   TDOAUpdate TDOA_protobuf;
   std::vector<bool> acomms_heard;

 private: // Configuration variables
   int  tdoa_id;
   double x_offset,y_offset;

 private: // State variables

};

#endif 
