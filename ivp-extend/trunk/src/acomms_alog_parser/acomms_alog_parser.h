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

#define PSK2_RECEIVE_DURATION 4
#define PSK1_RECEIVE_DURATION 4
#define FSK0_RECEIVE_DURATION 6.5
#define MINI_RECEIVE_DURATION .8

// functions that might be useful

// find the nearest entry by time, returns index and time diff
template <class T>
std::pair<int,double> findNearest(
		std::vector< std::pair<double,T> > item_list, double msg_time );

// get a range of values specified by time
template <class T>
std::vector< std::pair<double,T> > getRange(
		std::vector< std::pair<double,T> > item_list, double t_start, double t_end );

class ACOMMS_ALOG_PARSER {

public:
	class RECEPTION_EVENT {
	public:
		RECEPTION_EVENT() {
			receive_status = not_set;
			receive_start_time = -1;
			missing_stats = false;
			missing_frames = false;
		}

		std::string source_filename;
		int source_line;

		// this vehicle's name and id at time of receipt
		std::string vehicle_name;
		int vehicle_id;

		enum RECEIVE_STATUS {
			not_set = 0,
			received_fully,
			received_partial,
			all_bad,
			sync_loss,
			driver_inactive,
			no_frames
		};
		RECEIVE_STATUS receive_status;

		// usually related to the receive status
		std::string debugInfo;

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

		// DO NOT USE
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

		// DO NOT USE
		std::vector<std::pair<double, RECEPTION_EVENT> > reception_matches;
		int hasVehicleMatch( std::string vehicle_name );
	};

private:
	void outputResults();

	std::vector<std::string> vehicle_names;
	std::vector<TRANSMISSION_EVENT> all_transmissions;
	std::vector<RECEPTION_EVENT> all_receptions;
	std::map<std::string, std::vector<TRANSMISSION_EVENT> > vehicle_transmissions;
	std::map<std::string, std::vector<RECEPTION_EVENT> > vehicle_receptions;
	std::vector<RECEPTION_EVENT> unmatched_receptions;

	// double variable data histories by request
	std::map<std::string, std::map< std::string, std::vector< std::pair<double, double> > > > double_data;
	// string variable data histories by request
	std::map<std::string, std::map< std::string, std::vector< std::pair<double, std::string> > > > string_data;
	// keying is [vehicle_name][variable_name] --> vector<pair<time,val>>

	// default variable histories for each vehicle, sorted by time
	std::map<std::string, std::vector<std::pair<double, double> > >
		gps_x, gps_y; //, desired_thrust, voltage;
	std::map<std::string, std::vector<std::pair<double, std::string> > >
		acomms_driver_status, acomms_transmit_data, acomms_transmitted_data_hex,
		acomms_received_data_hex;
	std::map<std::string, std::vector<std::pair<double, boost::posix_time::ptime> > >
		gps_time;
	// keying is [vehicle_name] --> vector<pair<time,val>>

// --------------------------------------------------------------------------------
// Don't look past here or you will go blind ~ Josh
// --------------------------------------------------------------------------------

public:
	ACOMMS_ALOG_PARSER();
	~ACOMMS_ALOG_PARSER() {}

	void addStringVar( std::string varname ) {
		string_vars.push_back( varname );
	}
	void addDoubleVar( std::string varname ) {
		double_vars.push_back( varname );
	}

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
		void applyTimeOffset(double offset);

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

		// indexed by time
		std::vector<std::pair<double, double> > gps_x, gps_y;
		//, desired_thrust, voltage;
		std::vector<std::pair<double, std::string> > acomms_driver_status,
				acomms_transmit_data, acomms_transmitted_data_hex,
				acomms_received_data_hex, acomms_transmit_data_binary;
		std::vector<std::pair<double, boost::posix_time::ptime> > gps_time;

		std::map< std::string, std::vector<std::pair<double,double> > > double_series;
		std::map< std::string, std::vector<std::pair<double,std::string> > > string_series;
		std::vector<std::string> double_vars, string_vars;

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

		void processStatistics( RECEPTION_EVENT * r_event,
				micromodem::protobuf::ReceiveStatistics stat, int line_number );
		bool tryCombine( int frame_index, int stats_index );
		void fillStats( RECEPTION_EVENT * frame_event, RECEPTION_EVENT stats_event );
	};

private:
	std::vector<std::string> double_vars, string_vars;

	static ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings =
			false);
	static std::string getNextRawLine_josh(FILE *fileptr);

	double computeMatching(RECEPTION_EVENT * r_event,
			TRANSMISSION_EVENT * t_event);

	void publishSummary();
	void populateReceiveEvent( RECEPTION_EVENT * r_event, std::string name, double time );

	std::vector<FILE_INFO> alog_files;
};


#endif /* ACOMMS_ALOG_PARSER_H_ */
