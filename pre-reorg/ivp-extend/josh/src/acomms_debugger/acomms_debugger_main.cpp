/*
 * acomms_alog_parser_main.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */

// copy to your own tree before making modifications
// this version is subject to being changed or overwritten
using namespace std;

#include "JoshUtils.h"
#include <fstream>
#include <map>
//#include "LogUtils.h"

#define MAX_LINE_LENGTH 10000

const std::string delimiter = ";";

bool sort_func(std::pair<std::string,ALogEntry> e1, std::pair<std::string,ALogEntry> e2 ) {
	return e1.second.getTimeStamp() < e2.second.getTimeStamp();
}

int main(int argc, char *argv[]) {
	// Look for a request for version information
	if (scanArgs(argc, argv, "-h", "--help")) {
//		printHelp();
		return (0);
	}

	if (argc < 2) {
		cout << "Insufficient arguments - must provide input log file." << endl;
		return 0;
	}

	std::vector<std::string> filepaths;
	for ( int i=1; i<argc; i++ ) {
		if ( JoshUtil::wildCardMatch("*.alog", string(argv[i])) )
			filepaths.push_back(string(argv[i]));
		else
			JoshUtil::searchForFiles( filepaths,
					string(argv[i]),
					0,
					"*.alog");
	}

//	std::vector<FILE *> openfiles;
	for ( int i=0; i<filepaths.size(); i++ ) {
		FILE *logfile = fopen(argv[1], "r");
		if ( logfile == NULL ) {
			std::cout << "Error opening file " << filepaths[i] << std::endl;
			exit(0);
		}
		fclose(logfile);
	}

	std::vector<std::pair<std::string,ALogEntry> > data;
//	std::map<std::string,std::vector<ALogEntry> > data;
//	std::map<std::string,double> time_offset;
	for ( int i=0; i<filepaths.size(); i++ ) {
		FILE * logfile = fopen(filepaths[i].c_str(),"r");
		std::string file = filepaths[i];
		std::vector<ALogEntry> temp_data;
		double time_offset_sum = 0;
		int time_samples = 0;
		ALogEntry entry = JoshUtil::getNextRawALogEntry_josh(logfile);
//		std::cout << entry.getStatus() << std::endl;
		while ( entry.getStatus() != "eof" ) {
			std::string key = entry.getVarName();
			if ( key == "ACOMMS_DRIVER_STATUS" ||
				 key == "ACOMMS_RECEIVED_ALL" ||
				 key == "ACOMMS_BAD_FRAMES" ||
				 key == "ACOMMS_TRANSMITTED_DATA_HEX" ||
				 key == "ACOMMS_RECEIVED_DATA_HEX" ||
				 key == "TULIP_UPDATES" ||
				 key == "TULIP_DEBUG" ||
				 key == "ACOMMS_TRANSMIT_DATA_BINARY") {
				if ( entry.isNumerical() ) {
					std::stringstream ss;
					ss << entry.getDoubleVal();
					entry.set(
							entry.getTimeStamp(),
							entry.getVarName(),
							entry.getSource(),
							entry.getSrcAux(),
							ss.str() );

				}
				temp_data.push_back(entry);
			} else if ( key == "GPS_TIME_SECONDS" ) {
				time_offset_sum += entry.getDoubleVal()-entry.getTimeStamp();
				time_samples++;
			}
			entry = JoshUtil::getNextRawALogEntry_josh(logfile);
		}
		double time_offset = time_offset_sum/time_samples;
		std::cout << "time offset " << time_offset << " from " << time_samples <<
				" samples." << std::endl;

		std::cout << "Got " << temp_data.size() << " entries from " <<
				file << std::endl;

		for ( int j=0; j<temp_data.size(); j++ ) {
			entry = temp_data[j];
			entry.set(
					entry.getTimeStamp() + time_offset,
					entry.getVarName(),
					entry.getSource(),
					entry.getSrcAux(),
					entry.getStringVal() );
			data.push_back( std::pair<std::string,ALogEntry>(file, entry));
		}
	}

	std::sort(data.begin(), data.end(), sort_func);

	std::map<std::string,std::string> driver_status;
	std::ofstream output;
	output.open("acomms_debug.csv");
	output << "time" << delimiter;
	for ( int i=0; i<filepaths.size(); i++ ) {
		driver_status[filepaths[i]] = "not set";
		output << filepaths[i];
		if ( i < filepaths.size()-1 ) output << delimiter;
	}
	output << std::endl;

	for ( int i=0; i<data.size(); i++ ) {
		ALogEntry entry = data[i].second;
		std::string file = data[i].first;

		if (entry.getVarName() == "ACOMMS_DRIVER_STATUS") {
			if (driver_status[file] != entry.getStringVal()) {
				driver_status[file] = entry.getStringVal();
				output << entry.getTimeStamp() << delimiter;
				for ( int j=0; j<filepaths.size(); j++ ) {
					output << driver_status[ filepaths[j] ];
					if ( j<filepaths.size()-1 ) output << delimiter;
				}
				output << std::endl;
			}
		} else {
			std::map<std::string,std::string> tmp_data;
			tmp_data[file] = entry.getVarName() + ":  " +
					entry.getStringVal();
			output << entry.getTimeStamp() << delimiter;
			for ( int j=0; j<filepaths.size(); j++ ) {
				output << tmp_data[filepaths[j]];
				if ( j<filepaths.size()-1 ) output << delimiter;
			}
			output << std::endl;
		}
	}

	output.close();
	return 0;
}
