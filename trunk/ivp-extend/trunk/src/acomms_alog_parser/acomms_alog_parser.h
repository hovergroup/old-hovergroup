/*
 * acomms_alog_paser.h
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */


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

#ifndef ACOMMS_ALOG_PARSER_H_
#define ACOMMS_ALOG_PARSER_H_

#define UTC_TIME_OFFSET -4
#define MAX_LINE_LENGTH 10000
#define TRANSMISSION_TIMEOUT 10

class ACOMMS_ALOG_PARSER {
public:
	ACOMMS_ALOG_PARSER();
	~ACOMMS_ALOG_PARSER() {};

	void addAlogFile( std::string filename );
	void addAlogFile( boost::filesystem::path filepath );

	void runParser();

	class FILE_INFO {
	public:
		FILE_INFO() {}

		std::string filename;
		FILE * logfile;
		std::vector<std::string> header_lines;

		boost::posix_time::ptime creation_time;
		double moos_time_offset;
		std::string vehicle_name;
		int vehicle_id;

		ALogEntry getNextEntry();
		ALogEntry getTimeAdjustedNextEntry();

		std::string getNextLine();
		void parseMOOSFile();
		void parseHeaderLines();

		bool offsetViaGPS();
		void offsetViaHeader();

		void resetFile();
	};

	class VEHICLE_HISTORY {
	public:
		std::vector<FILE_INFO> vehicle_logs;

		ALogEntry getNextEntry();

	};

	class RECEPTION_EVENT {
	public:
		RECEPTION_EVENT() {}

		std::string vehicle_name;
		int vehicle_id;

		int receive_status;
		// 0 = received fully
		// 1 = sync ok, bad crc(s)
		// 2 = sync loss, no detection

		double receive_time, receive_start_time;

		int gps_x, gps_y;
		double gps_age;

		goby::acomms::protobuf::ModemTransmission data_msg;
	};

	class TRANSMISSION_EVENT {
	public:
		TRANSMISSION_EVENT() {}

		std::string transmitter_name;
		int rate, source_id, destination_id;
		std::string data;

		int gps_x, gps_y;
		double gps_age;

		double transmission_time;

		std::map<std::string,RECEPTION_EVENT> receptions_map;
		std::vector<RECEPTION_EVENT> receptions_vector;
	};

private:
	void parseAllHeaders();
	void parseMOOSFiles();
	void generateHistories();
	void lookForEvents();

	static ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings=false);

	std::vector<FILE_INFO> alog_files;
	std::vector<std::string> vehicle_names;
	std::map<std::string,VEHICLE_HISTORY> vehicle_histories;

	std::vector<RECEPTION_EVENT> initial_receive_events;
	std::vector<TRANSMISSION_EVENT> initial_transmit_events;
};


#endif /* ACOMMS_ALOG_PARSER_H_ */
