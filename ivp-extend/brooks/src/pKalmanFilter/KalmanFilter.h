/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: KalmanFilter.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef KalmanFilter_HEADER
#define KalmanFilter_HEADER

#include "MOOSLib.h"
#include <gsl/gsl_matrix.h>

using namespace std;

class KalmanFilter : public CMOOSApp
{
public:
	KalmanFilter();
	virtual ~KalmanFilter();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

	void GetWaypoints();
	void GetMatrices(string);
	void GetDesiredHeading();
	void GetCrossTrackError();

protected:
	// insert local vars here
	gsl_matrix *A,*B,*B_noise,*C,*Q,*R; //inputs
	gsl_matrix *z,*x_pre,*P_pre; 		//predicted
	gsl_matrix *K,*x_hat,*P; 			//computed
	gsl_matrix *x_hist;					//history
	double u_hist;						//command history

	vector<double> wpx,wpy;
	double x1,y1,x2,y2;
	double myx,myy,myheading;
	double start_time,segment_time;
	bool begin;
};

#endif 
