/*
 * acomms_alog_paser.h
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */

#ifndef ACOMMS_ALOG_PARSER_H_
#define ACOMMS_ALOG_PARSER_H_

#include "goby/acomms/modem_driver.h"
#include "goby/acomms/protobuf/mm_driver.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include "LogUtils.h"
#include "MBUtils.h"
#include "ProcessConfigReader.h"
#include <stdio.h>
#include <vector>
#include <time.h>
#include <map>
#include <math.h>
#include <algorithm>
#include "acomms_messages.h"
#include <stdlib.h>

#define UTC_TIME_OFFSET -4

class ACOMMS_ALOG_PARSER {

public:

	bool is_empty(std::ifstream&);

	class RECEPTION_EVENT {
	public:
		RECEPTION_EVENT() {
			receive_status = -1;
			receive_start_time = -1;
			missing_stats = false;
			missing_frames = false;
		}

		std::string source_filename;
		int source_line;

		// this vehicle's name and id at time of receipt
		std::string vehicle_name;
		int vehicle_id;

		// only sync loss implemented
		int receive_status;
		// 0 = received fully
		// 1 = sync ok, bad crc(s)
		// 2 = sync loss, no detection

		// parsed out
		int rate, num_frames, source_id;
		std::vector<int> frame_status;
		// 0 = doesn't exist
		// 1 = good
		// 2 = bad

		// time receipt was published by driver
		double receive_time;
		// time micromodem reported hearing start of packet
		double receive_start_time;

		// gps information at time of receipt
		int gps_x, gps_y;
		double gps_age;

		// packet information
		goby::acomms::protobuf::ModemTransmission data_msg;

		std::string received_data_hex;

		// used during initial parsing
		bool missing_stats;
		bool missing_frames;
	};

	class TRANSMISSION_EVENT {
	public:
		TRANSMISSION_EVENT() {
		}

		std::string source_filename;
		int source_line;

		// name and id of transmitting vehicle
		std::string transmitter_name;
		int source_id;

		// transmission rate
		int rate;
		// -1 = fsk mini
		// 0 = fsk
		// 2 = psk

		int num_frames;

		int destination_id;
		// 0 = broadcast

		// data_sent in transmission, binary data not implemented
		std::string data;
		std::string transmitted_data_hex;

		// gps information at time of transmission
		int gps_x, gps_y;
		double gps_age;

		// transmission time
		double transmission_time;

		// reception events are stored in both a map and vector for ease of access
		// they contain the same reception events
		std::map<std::string, RECEPTION_EVENT> receptions_map;
		std::vector<RECEPTION_EVENT> receptions_vector;

		// used when performing matching
		std::vector<std::pair<int, RECEPTION_EVENT> > reception_matches;
	};

private:
	void outputResults();

	std::vector<std::string> vehicle_names;
	std::vector<TRANSMISSION_EVENT> all_transmissions;
	std::vector<RECEPTION_EVENT> all_receptions;
	std::map<std::string, std::vector<TRANSMISSION_EVENT> > vehicle_transmissions;
	std::map<std::string, std::vector<RECEPTION_EVENT> > vehicle_receptions;

// --------------------------------------------------------------------------------
// Don't look past here or you will go blind ~ Josh
// --------------------------------------------------------------------------------

public:
	ACOMMS_ALOG_PARSER();
	~ACOMMS_ALOG_PARSER() {
	}
	;

	void addAlogFile(std::string filename);
	void addAlogFile(boost::filesystem::path filepath);

	void runParser();

	class FILE_INFO {
	public:
		FILE_INFO(std::string file_name) {
			filename = file_name;
			file_is_open = false;
		}

		void processFile();

		double getMOOSTimeOffset() {
			return moos_time_offset;
		}
		std::string getVehicleName() {
			return vehicle_name;
		}
		std::string getFilename() {
			return filename;
		}

		std::vector<std::pair<double, RECEPTION_EVENT> > receptions;
		std::vector<std::pair<double, TRANSMISSION_EVENT> > transmissions;

	private:
		void parseHeaderLines();
		void parseMOOSFile();
		void collectData();
		bool offsetViaGPS();
		void offsetViaHeader();

		RECEPTION_EVENT constructReception(double msg_time, int line_number,
				std::string msg_val);
		TRANSMISSION_EVENT constructTransmission(double msg_time,
				int line_number, std::string msg_val);

		ALogEntry getNextEntry();
		std::string getNextLine();

		bool file_is_open;
		void openFile();
		void closeFile();

		std::string filename;
		FILE * logfile;

		// parsed from moos file
		std::string vehicle_name;
		int vehicle_id;

		double moos_time_offset;
		std::vector<std::string> header_lines;
		boost::posix_time::ptime creation_time;

		// indexed by time
		std::vector<std::pair<double, double> > gps_x, gps_y, desired_thrust,
				voltage;
		std::vector<std::pair<double, std::string> > acomms_driver_status,
				acomms_transmit_data, acomms_transmitted_data_hex,
				acomms_received_data_hex;
		std::vector<std::pair<double, boost::posix_time::ptime> > gps_time;

		void processStatistics( RECEPTION_EVENT * r_event,
				micromodem::protobuf::ReceiveStatistics stat, int line_number );
		bool tryCombine( int frame_index, int stats_index );
		void fillStats( RECEPTION_EVENT * frame_event, RECEPTION_EVENT stats_event );
	};

	class VEHICLE_HISTORY {
	public:
		std::vector<FILE_INFO> vehicle_logs;

		ALogEntry getNextEntry();

	};

private:
	static ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings =
			false);
	static std::string getNextRawLine_josh(FILE *fileptr);

	double computeMatching(RECEPTION_EVENT * r_event,
			TRANSMISSION_EVENT * t_event);

	std::vector<FILE_INFO> alog_files;
};

#endif /* ACOMMS_ALOG_PARSER_H_ */
