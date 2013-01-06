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
#include "MOOS/libMOOS/MOOSLib.h"
#include "XYVector.h"
#include "XYSegList.h"
#include "math.h"
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
        std::string msg = entry.getStringVal();
        bool update = false;

        if ( key == "VIEW_POINT" ) {
            if ( msg.find("singapore_waypoint", 0) != std::string::npos ) {

                MOOSChomp(msg, "x=");
                std::string sline = MOOSChomp(msg,",y=");
                double target_x = atof(sline.c_str());
                sline = MOOSChomp(msg,",");
                double target_y = atof(sline.c_str());

                ALogEntry new_entry;
                new_entry.set(
                        entry.getTimeStamp(),
                        "WAYPOINT_X",
                        entry.getSource(),
                        entry.getSrcAux(),
                        target_x);
                new_data.push_back(new_entry);
                new_entry.set(
                        entry.getTimeStamp(),
                        "WAYPOINT_Y",
                        entry.getSource(),
                        entry.getSrcAux(),
                        target_y);
                new_data.push_back(new_entry);

                std::cout << "found waypoint: " << target_x << ", " << target_y << std::endl;
            }
        }
        new_data.push_back(entry);
        entry = JoshUtil::getNextRawALogEntry_josh(logfile);
    }

    fclose(logfile);

    std::ifstream infile;
    infile.open(argv[1]);

    std::ofstream outfile;
    std::string filename = "edited_" + std::string(argv[1]);
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
