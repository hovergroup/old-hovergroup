/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef acomms_driver_HEADER
#define acomms_driver_HEADER

#include "MOOSLib.h"
#include "goby/acomms/modem_driver.h"
#include "goby/util/binary.h"
#include "goby/common/logger.h"
#include "goby/acomms/connect.h"
#include "XYRangePulse.h"

class acomms_driver : public CMOOSApp
{
public:
	acomms_driver();
	virtual ~acomms_driver();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	google::protobuf::uint32 my_id;
	std::string port_name, my_name;
	bool use_psk_for_minipackets;
	double m_navx, m_navy;

	double transmission_pulse_range, transmission_pulse_duration, receive_pulse_range,
		receive_pulse_duration;

	int transmission_rate, transmission_dest;
	std::string transmission_data;

	goby::acomms::ModemDriverBase* driver;
	goby::acomms::protobuf::DriverConfig cfg;

	void transmit_data();
	void handle_data_receive( const goby::acomms::protobuf::ModemTransmission& data_msg );
	void publishReceivedInfo( goby::acomms::protobuf::ModemTransmission trans, int index );
	void handle_raw_incoming( const goby::acomms::protobuf::ModemRaw& msg );
	bool RXD_received, CST_received;

	void startDriver( std::string logDirectory );
	bool driver_ready, driver_initialized;
	std::string status;
	double status_set_time, receive_set_time, transmit_set_time;

	void publishWarning( std::string message );
	void publishStatus( std::string status_update );
	void postRangePulse( std::string label, double range, double duration, std::string color = "yellow" );

	void RegisterVariables();

	bool file_exists( std::string filename );

	std::ofstream verbose_log;
};

#endif 
