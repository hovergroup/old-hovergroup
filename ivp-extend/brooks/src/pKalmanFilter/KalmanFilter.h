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


class KalmanFilter : public CMOOSApp
{
public:
	KalmanFilter();
	virtual ~KalmanFilter();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here

};

#endif 
