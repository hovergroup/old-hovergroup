/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommsController.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommsController_HEADER
#define AcommsController_HEADER

#include "MOOSLib.h"
#include <vector>
#include <boost/tokenizer.hpp>
#include <gsl/gsl_blas.h>

class AcommsController : public CMOOSApp
{
public:
	AcommsController();
	virtual ~AcommsController();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// configuration values
	double m_transmit_period, m_receive_delay;

	// temp stores, don't use
	std::string received_data, driver_status;
	double last_transmit_time, last_receive_time;
	bool signal_sync_loss;

	// current kayak state, updated continuously
	double m_filtered_heading, m_unfiltered_heading, m_gps_x, m_gps_y, m_gps_speed;

	// current waypoint target
	double m_target_x, m_target_y;
	void parseWaypoint( std::string msg );

	// your matrices
	gsl_matrix *matrix_A, *matrix_B;
	void readMatrices(); // read matrices from moos file

	bool active;
	void onActive(); // called when controller is enabled
	void onInactive(); // called when controller is disabled

	void onReceive( std::string data ); // when data is received
	void onLoss(); // when data was not received, or bad data received
	void onTransmit(); // called on transmit duty cycle

	void publishWarning( std::string msg );
	gsl_matrix * generateMatrix( std::string msg );
	std::vector<std::string> tokenize( std::string msg, std::string tokens );
};

#endif 

// MISSION_MODE = ACOMMS_CONTROL to active
// post x,y to ACOMMS_CONTROLLER_TARGET to set waypoint
// warning posted to ACOMMS_CONTROLLER_WARNING
// current waypoint posted to ACOMMS_CONTROLLER_WAYPOINT
