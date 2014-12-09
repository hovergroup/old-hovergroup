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
#include "XYFormatUtilsSegl.h"
#include "XYSegList.h"
#include "XYFormatUtilsRangePulse.h"
//#include "LogUtils.h"

#define MAX_LINE_LENGTH 10000

const std::string delimiter = ";";

bool sort_func(std::pair<std::string,ALogEntry> e1, std::pair<std::string,ALogEntry> e2 ) {
	return e1.second.getTimeStamp() < e2.second.getTimeStamp();
}

int main(int argc, char *argv[]) {
    double navx, navy;
    int count = 0;

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

	ALogEntry entry;
	for (int i=0; i<10; i++) {
	    entry = JoshUtil::getNextRawALogEntry_josh(logfile);
	}


//    ALogEntry x_entry, y_entry;
//    x_entry.set(entry.getTimeStamp(), "NAV_X", "dummy", "dummy", 0.0);
//    y_entry.set(entry.getTimeStamp(), "NAV_Y", "dummy", "dummy", 1000.0);
//    new_data.push_back(x_entry);
//    new_data.push_back(y_entry);

//		std::cout << entry.getStatus() << std::endl;
    while ( entry.getStatus() != "eof" ) {
        iteration ++;
        std::string key = entry.getVarName();
        if (entry.getStatus() == "invalid") {
            // do nothing
        } else if (key == "VIEW_RANGE_PULSE") {
            // do nothing
        } else {
            new_data.push_back(entry);
        }
        entry = JoshUtil::getNextRawALogEntry_josh(logfile);
//        } else if (key == "NAV_X") {
//            navx = entry.getDoubleVal();
//            count++;
//        } else if (key == "NAV_Y") {
//            navy = entry.getDoubleVal();
//            count++;
//        }
//
//        if (count >= 2) {
//            stringstream marker;
//            marker << "x=" << navx;
//            marker << ",y=" << navy;
//            marker << ",type=square,color=red,label=TARGET_NAV";
//
//            ALogEntry new_entry;
//            new_entry.set(
//                    entry.getTimeStamp(),
//                    "VIEW_MARKER",
//                    entry.getSource(),
//                    entry.getSrcAux(),
//                    marker.str());
//
//            new_data.push_back(new_entry);
//            count = 0;
//        } else {
//            new_data.push_back(entry);
//        }
//        entry = JoshUtil::getNextRawALogEntry_josh(logfile);
    }

    fclose(logfile);

    std::ifstream infile;
    infile.open(argv[1]);

    std::ofstream outfile;
    std::string filename = "tdoaEdit_" + std::string(argv[1]);
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
