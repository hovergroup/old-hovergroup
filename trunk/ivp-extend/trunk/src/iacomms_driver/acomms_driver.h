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
	std::string port_name;

	goby::acomms::ModemDriverBase* driver;
	goby::acomms::protobuf::DriverConfig cfg;

	void handle_data_receive( const goby::acomms::protobuf::ModemTransmission& data_msg );

	void startDriver( std::string logDirectory );
	bool driver_ready;

	std::ofstream verbose_log;
};

#endif 
