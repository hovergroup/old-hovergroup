/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver_sim.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef acomms_driver_sim_HEADER
#define acomms_driver_sim_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "goby/acomms/modem_driver.h"
#include "goby/util/binary.h"
#include "goby/common/logger.h"
#include "goby/acomms/connect.h"
#include <acomms_messages.h>
#include <sstream>
#include "XYRangePulse.h"


class acomms_driver_sim : public CMOOSApp
{
public:
	acomms_driver_sim();
	virtual ~acomms_driver_sim();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:

	//Sims
	double x, y;
	double m_navx, m_navy;

	double sending_time,channel_delay;
	double sent_time, start_time;
	bool transmitting, receiving;
	std::vector<double> getProbabilities(double,double,double,double,int);
	bool rollDice(double);

	double transmission_pulse_range, transmission_pulse_duration, receive_pulse_range,
			receive_pulse_duration;

	// insert local vars here
	google::protobuf::uint32 my_id;
	std::string my_name;

	int transmission_rate, transmission_dest;
	std::string transmission_data;
	std::string sent_data;

	void transmit_data( bool isBinary );
	void handle_data_receive(std::string);
	bool RXD_received, CST_received;

	void startDriver( std::string logDirectory );
	bool driver_ready, driver_initialized;
	std::string status;
	double status_set_time, receive_set_time;

	void publishWarning( std::string message );
	void publishStatus( std::string status_update );
	void postRangePulse( std::string label, double range, double duration, std::string color = "yellow" );

	void RegisterVariables();

};

#endif 
