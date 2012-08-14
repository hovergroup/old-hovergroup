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
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <math.h>

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

	void PublishStates();
	void EstimateStates();
	void UpdateSensorReadings();

	double GetDesiredHeading();
	double GetCrossTrackError();
	double GetDistance(double,double,double,double);

protected:
	// insert local vars here
	gsl_matrix *A,*B,*B_noise, *B_in, *C,*Q,*R; 	//matrix inputs
	gsl_vector *z,*x_pre,*x_hat,*x_hist;			//vectors
	gsl_matrix *P_pre, *K, *P, *P_hist; 			//computed
	double u,u_hist;								//command history

	vector<double> wpx,wpy,time,headings;
	double x1,y1,x2,y2;
	double speed,thrust;
	double myx,myy,myheading;
	double start_time,wp_id, wait, offset;
	bool begin,end;
};

#endif 
