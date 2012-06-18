/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver_sim.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef acomms_driver_sim_HEADER
#define acomms_driver_sim_HEADER

#include "MOOSLib.h"
#include "goby/acomms/modem_driver.h"
#include "goby/util/binary.h"
#include "goby/common/logger.h"
#include "goby/acomms/connect.h"

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

//	struct ModemStat{
//		double time,rtime,doppler;
//		int mfd_peak,mfd_power,mfd_ratio,spl,shf_agn;
//		int shf_ainpshift,shf_ainshift,shf_mfdshift,shf_p2bshift,rate;
//		int source,dest,psk_error_code,number_frames,number_bad_frames,snr_rss;
//		int snr_in, snr_out,snr_symbols, mse_equalizer,data_quality_factor;
//		int stddev_noise,carrier_freq,bandwidth;
//		std::string time_source,mode,clock_mode,packet_type;
//
//		//time: 1338220425000000<|>time_source: MODEM_TIME<|>
//				//[micromodem.protobuf.receive_stat] {<|>
//				//mode: RECEIVE_GOOD<|>  time: "155345.0000"<|>
//				//clock_mode: NO_SYNC_TO_PPS_AND_CCCLK_BAD<|>
//				//mfd_peak: 315<|>  mfd_power: 12<|>
//				//mfd_ratio: 138<|>  spl: 155<|>  shf_agn: 25<|>  shf_ainpshift: 0<|>
//				//shf_ainshift: 0<|>  shf_mfdshift: 2<|>  shf_p2bshift: 2<|>  rate: 0<|>
//				//source: 2<|>  dest: 0<|>  psk_error_code: 0<|>  packet_type: FSK<|>
//				//number_frames: 1<|>  number_bad_frames: 0<|>  snr_rss: -1<|>  snr_in: -1<|>
//				//snr_out: -1<|>  snr_symbols: -1<|>  mse_equalizer: 0<|>  data_quality_factor: 238<|>
//				//doppler: 0.2<|>  stddev_noise: -1<|>  carrier_freq: 25120<|>  bandwidth: 4000<|>
//				//version: 0<|>}<|>
//	};

	//Sims
	double x, y;
	double encoding_time,sending_time;
	double sent_time, start_time;
	bool transmitting, receiving;
	void handle_data_receive(std::string);
	std::vector<double> getProbabilities(double,double,double,double,double);
	bool rollDice(double);
	std::string SerializeToString(ModemStat);

	// insert local vars here
	google::protobuf::uint32 my_id;
	std::string my_name;

	int transmission_rate, transmission_dest;
	std::string transmission_data;
	std::string sent_data;

//	goby::acomms::protobuf::DriverConfig cfg;

	void transmit_data( bool isBinary );
	//void handle_data_receive( const goby::acomms::protobuf::ModemTransmission& data_msg );
	//void publishReceivedInfo( goby::acomms::protobuf::ModemTransmission trans, int index );
	//void handle_raw_incoming( const goby::acomms::protobuf::ModemRaw& msg );

	bool RXD_received, CST_received;

	void startDriver( std::string logDirectory );
	bool driver_ready, driver_initialized;
	std::string status;
	double status_set_time, receive_set_time;

	void publishWarning( std::string message );
	void publishStatus( std::string status_update );
	void RegisterVariables();

//	std::ofstream verbose_log;
};

#endif 
