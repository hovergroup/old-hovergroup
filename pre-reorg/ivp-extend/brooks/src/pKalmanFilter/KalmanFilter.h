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

	void GetWaypoints(string);
	void GetMatrices(string);

	void PublishStates();
	void PublishSegList();
	void EstimateStates();
	void UpdateSensorReadings();

	double Constrain(double, double, double);
	double GetDesiredHeading();
	double GetHeading(double,double,double,double);
	double GetCrossTrackError();
	double GetDistance(double,double,double,double);

protected:
	// insert local vars here
	gsl_matrix *A,*B,*B_noise,*C,*Q,*R; 	//matrix inputs
	gsl_vector *z,*x_pre,*x_hat,*x_hist;	//vectors
	gsl_matrix *P_pre, *K, *P, *P_hist; 	//computed
	double u;								//command

	double x_size, z_size;
	double constrain_max, constrain_min;
	vector<double> wpx1,wpy1,wpx2,wpy2,time,headings;
	double x1,y1,x2,y2;
	double speed,thrust,compass_offset,rudder_offset;
	double myx,myy,myheading;
	double start_time,wp_id, wait, offset, crosstrack;
	double timer;
	bool begin,end;
};

#endif 
