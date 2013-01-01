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
#include "XYSegList.h"
#include "math.h"
//#include "LogUtils.h"

#define MAX_LINE_LENGTH 10000

const std::string delimiter = ";";

bool sort_func(std::pair<std::string,ALogEntry> e1, std::pair<std::string,ALogEntry> e2 ) {
	return e1.second.getTimeStamp() < e2.second.getTimeStamp();
}

double target_x, target_y;
double nostromo_x, nostromo_y;
double kassandra_x, kassandra_y;

int pulses=0;

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

        if ( key == "VIEW_MARKER" ) {
            if ( msg.find("label=target", 0) != std::string::npos ) {
                int index1 = msg.find("msg=",0);
                msg = msg.substr(0,index1+10);

                ALogEntry new_entry;
                new_entry.set(
                        entry.getTimeStamp(),
                        "VIEW_MARKER",
                        entry.getSource(),
                        entry.getSrcAux(),
                        msg);

                new_data.push_back(new_entry);

                MOOSChomp(msg, "x=");
                std::string sline = MOOSChomp(msg,",y=");
                target_x = atof(sline.c_str());
                sline = MOOSChomp(msg,",");
                target_y = atof(sline.c_str());

                std::cout << "target: " << target_x << ", " << target_y << std::endl;
            }
            update = true;
        } else if ( key == "NODE_REPORT") {
            bool nostromo = false;
            if ( msg.find("NOSTROMO",0) != std::string::npos )
                nostromo = true;

            std::string sline = MOOSChomp(msg,"X=");
            sline = MOOSChomp(msg,",Y=");
            double x = atof(sline.c_str());
            sline = MOOSChomp(msg,",");
            double y = atof(sline.c_str());
            if ( nostromo ) {
                nostromo_x = x;
                nostromo_y = y;
            } else {
                kassandra_x = x;
                kassandra_y = y;
            }
            update = true;
        }

        if (update) {
            XYSegList seg;
            seg.add_vertex(target_x, target_y);
            seg.add_vertex(nostromo_x, nostromo_y);
            seg.add_vertex(kassandra_x, kassandra_y);
            seg.add_vertex(target_x, target_y);
            seg.set_label("triangle");

            ALogEntry new_entry;
            new_entry.set(
                    entry.getTimeStamp(),
                    "VIEW_SEGLIST",
                    "josh",
                    "josh",
                    seg.get_spec());
            new_data.push_back(new_entry);

        }

        new_data.push_back(entry);
        entry = JoshUtil::getNextRawALogEntry_josh(logfile);
    }

    fclose(logfile);

    std::ifstream infile;
    infile.open(argv[1]);

    std::ofstream outfile;
    std::string filename = "triangled_" + std::string(argv[1]);
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
