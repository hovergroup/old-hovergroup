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
#include "math.h"
//#include "LogUtils.h"

#define MAX_LINE_LENGTH 10000

const std::string delimiter = ";";

bool icarus = false;

bool sort_func(std::pair<std::string,ALogEntry> e1, std::pair<std::string,ALogEntry> e2 ) {
	return e1.second.getTimeStamp() < e2.second.getTimeStamp();
}

double target_x, target_y;
double estimate_x, estimate_y, estimate_h;
double gps_heading;

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

    if ( std::string(argv[1]).find("ICARUS",0) != std::string::npos ) {
        icarus = true;
    }

    int iteration = 0;
	std::vector<ALogEntry> new_data;
    ALogEntry entry = JoshUtil::getNextRawALogEntry_josh(logfile);
//		std::cout << entry.getStatus() << std::endl;
    while ( entry.getStatus() != "eof" ) {
        iteration ++;
        std::string key = entry.getVarName();

        if ( key == "TARGET_EST_H" ) {
            estimate_h = entry.getDoubleVal();
        } else if ( key == "TARGET_EST_X" ) {
//            std::cout << "est x " << entry.getStringVal() << std::endl;
            estimate_x = entry.getDoubleVal();
        } else if ( key == "TARGET_EST_Y" ) {
            estimate_y = entry.getDoubleVal();
        } else if ( key == "GPS_HEADING" ) {
            gps_heading = entry.getDoubleVal();
        }

        if ( key == "VIEW_MARKER" ) {
            if ( !icarus ) {
                std::string msg = entry.getStringVal();
    //            std::cout << msg << std::endl;
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
                } else if ( msg.find("label=estimate", 0) != std::string::npos ) {
                    int index1 = msg.find("msg=",0);
                    msg = msg.substr(0,index1+9);

                    double x_error = estimate_x - target_x;
                    double y_error = estimate_y - target_y;
                    double error = sqrt( pow(x_error,2) + pow(y_error,2) );
                    std::cout << "est: " << estimate_x << ", " << estimate_y << ", " << error << std::endl;

                    char buffer [10];
                    sprintf(buffer, "%d", (int) error);
                    msg += std::string(buffer);

                    ALogEntry new_entry;
                    new_entry.set(
                            entry.getTimeStamp(),
                            "VIEW_MARKER",
                            entry.getSource(),
                            entry.getSrcAux(),
                            msg);

                    new_data.push_back(new_entry);

                    XYVector vec;
                    vec.set_label("estimate_heading");
                    vec.setPosition(estimate_x, estimate_y);

                    double heading = estimate_h;
                    double dx = 5*sin(heading);
                    double dy = 5*cos(heading);

                    vec.setVectorXY(dx,dy);
                    vec.setHeadSize(3);

                    std::string vec_s = vec.get_spec();

                    new_entry.set(
                            entry.getTimeStamp(),
                            "VIEW_VECTOR",
                            entry.getSource(),
                            entry.getSrcAux(),
                            vec_s);

                    new_data.push_back(new_entry);
                }
            } else {
                std::string msg = entry.getStringVal();
                if ( msg.find("label=target", 0) != std::string::npos ) {
                    MOOSChomp(msg, "x=");
                    std::string sline = MOOSChomp(msg,",y=");
                    target_x = atof(sline.c_str());
                    sline = MOOSChomp(msg,",");
                    target_y = atof(sline.c_str());

                    std::cout << "target: " << target_x << ", " << target_y << std::endl;

                    XYVector vec;
                    vec.set_label("target_heading");
                    vec.setPosition(target_x,target_y);

                    double heading = M_PI/180.0 * gps_heading;
                    double dx = 5*sin(heading);
                    double dy = 5*cos(heading);

                    vec.setVectorXY(dx,dy);
                    vec.setHeadSize(3);

                    std::string vec_s = vec.get_spec();

                    ALogEntry new_entry;
                    new_entry.set(
                            entry.getTimeStamp(),
                            "VIEW_VECTOR",
                            entry.getSource(),
                            entry.getSrcAux(),
                            vec_s);

                    new_data.push_back(new_entry);
                }
            }
        } else if ( key == "VIEW_RANGE_PULSE" && !icarus) {
            std::string msg = entry.getStringVal();
            int index1 = msg.find(",edge_color");
            std::stringstream ss;
            ss << pulses;
            pulses++;
            msg.insert(index1,ss.str());

            if ( pulses > 5 ) pulses=0;

            ALogEntry new_entry;
            new_entry.set(
                    entry.getTimeStamp(),
                    "VIEW_RANGE_PULSE",
                    entry.getSource(),
                    entry.getSrcAux(),
                    msg);

            new_data.push_back(new_entry);

        } else if ( iteration > 10 /*&& key != "VIEW_SEGLIST" */ && key != "NAV_X" && key != "NAV_Y" ) {
            new_data.push_back(entry);
        }
        entry = JoshUtil::getNextRawALogEntry_josh(logfile);
    }

    ALogEntry new_entry;
//    new_entry.set(
//            0,
//            "VIEW_GRID",
//            "josh",
//            "josh",
//            "pts={-30,-290:280,-290:280,20:-30,20},cell_size=10,label=pag");
//    new_data.insert(new_data.begin(), new_entry);
    new_entry.set(
            0,
            "NAV_X",
            "josh",
            "josh",
            0);
    new_data.insert(new_data.begin(), new_entry);
    new_entry.set(
            0,
            "NAV_Y",
            "josh",
            "josh",
            1000);
    new_data.insert(new_data.begin(), new_entry);

    fclose(logfile);

    std::ifstream infile;
    infile.open(argv[1]);

    std::ofstream outfile;
    std::string filename = "relabeled_" + std::string(argv[1]);
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
