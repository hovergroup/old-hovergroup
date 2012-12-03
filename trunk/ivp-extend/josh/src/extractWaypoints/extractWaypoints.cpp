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
//#include "MBUtils.h"
#include "MOOSLib.h"
#include "XYVector.h"
//#include "LogUtils.h"

#define MAX_LINE_LENGTH 10000

const std::string delimiter = ";";

bool sort_func(std::pair<std::string,ALogEntry> e1, std::pair<std::string,ALogEntry> e2 ) {
	return e1.second.getTimeStamp() < e2.second.getTimeStamp();
}

double range = -1;

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

    FILE *logfile = fopen(argv[1], "r+");
    if ( logfile == NULL ) {
        std::cout << "Error opening file " << std::string(argv[1]) << std::endl;
        exit(0);
    }

    int iteration = 0;
	std::vector<ALogEntry> new_data;
    ALogEntry entry = JoshUtil::getNextRawALogEntry_josh(logfile);
//		std::cout << entry.getStatus() << std::endl;
    while ( entry.getStatus() != "eof" ) {
        iteration ++;
        std::string key = entry.getVarName();
        if ( iteration > 10 /*&& key != "VIEW_SEGLIST" */) {
            new_data.push_back(entry);
        }
        if ( key == "VIEW_POINT" &&
                entry.getStringVal().find("next waypoint") != std::string::npos ) {
            std::string sline = entry.getStringVal();
            std::string s = MOOSChomp(sline, "=");
            s = MOOSChomp(sline, ",");
            double x = atof(s.c_str());
            s = MOOSChomp(sline, "=");
            s = MOOSChomp(sline, ",");
            double y = atof(s.c_str());

            ALogEntry x_entry;
            ALogEntry y_entry;
            x_entry.set(
                    entry.getTimeStamp(),
                    "WAYPOINT_X",
                    "extractWaypoints",
                    entry.getSrcAux(),
                    x);
            y_entry.set(
                    entry.getTimeStamp(),
                    "WAYPOINT_Y",
                    "extractWaypoints",
                    entry.getSrcAux(),
                    y);
            new_data.push_back(x_entry);
            new_data.push_back(y_entry);
        } else if ( key == "TULIP_UPDATES" &&
                entry.getStringVal().find("range to target:") != std::string::npos ) {
            std::string sline = entry.getStringVal();
            MOOSChomp(sline,"range to target: ");
            double new_range = atof(sline.c_str());
            if ( new_range == range ) {
                range = new_range + 0.001;
            } else {
                range = new_range;
            }
            ALogEntry range_entry;
            range_entry.set(
                    entry.getTimeStamp(),
                    "TULIP_TARGET_RANGE",
                    "extractWaypoints",
                    entry.getSrcAux(),
                    range);
            new_data.push_back(range_entry);
        }
        entry = JoshUtil::getNextRawALogEntry_josh(logfile);
    }

    fclose(logfile);

    std::ifstream infile;
    infile.open(argv[1]);

    std::ofstream outfile;
    std::string filename = "waypointed_" + std::string(argv[1]);
    outfile.open(filename.c_str());

    for ( int i=0; i<5; i++ ) {
        std::string sline;
        std::getline(infile,sline);
        outfile << sline << std::endl;
    }

    for ( int i=0; i<new_data.size(); i++ ) {
        ALogEntry new_entry = new_data[i];

        std::stringstream ss;
        ss << new_entry.getTimeStamp() << "\t";
        ss << new_entry.getVarName() << "\t";
        ss << new_entry.getSource() << "\t";
        if ( new_entry.isNumerical() ) {
            ss << new_entry.getDoubleVal() << "\n";
        } else {
            ss << new_entry.getStringVal() << "\n";
        }

        outfile << ss.str();
    }

    infile.close();
    outfile.close();

	return 0;
}
