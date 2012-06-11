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

#ifndef ACOMMS_ALOG_PARSER_H_
#define ACOMMS_ALOG_PARSER_H_

#define UTC_TIME_OFFSET = -4;

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

		std::string getNextLine();
		void parseMOOSFile();
		void parseHeaderLines();

		bool offsetViaGPS();
		void offsetViaHeader();
	};

	class VEHICLE_HISTORY {
	public:
		std::vector<FILE_INFO> vehicle_logs;

		ALogEntry getNextEntry();

	};

	class TRANSMISSION_EVENT {
	public:
		TRANSMISSION_EVENT() {}


	};

private:
	void parseAllHeaders();
	void parseMOOSFiles();
	void generateHistories();

	std::vector<FILE_INFO> alog_files;
	std::map<std::string,VEHICLE_HISTORY> vehicle_histories;

};


#endif /* ACOMMS_ALOG_PARSER_H_ */
