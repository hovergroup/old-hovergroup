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
        if ( iteration > 10 ) {
            new_data.push_back(entry);
        }
        if ( key == "VIEW_SEGLIST" && entry.getStringVal().find("label=")!=std::string::npos ) {
//            std::cout << entry.getStringVal() << std::endl;
            int label_index = entry.getStringVal().find("label=",0);
            int label_end = entry.getStringVal().find(",",label_index);
            std::string label = entry.getStringVal().substr(label_index+6,label_end-label_index-6) + "v";
//            std::cout << label << std::endl;

            int points_start = entry.getStringVal().find("{",0);
            int points_end = entry.getStringVal().find("}",0);
            std::string points = entry.getStringVal().substr(points_start,points_end-points_start);
//            std::cout << points << std::endl;
            double x1 = atof(MOOSChomp(points,",").c_str());
            double y1 = atof(MOOSChomp(points,":").c_str());
            double x2 = atof(MOOSChomp(points,",").c_str());
            double y2 = atof(MOOSChomp(points,":").c_str());

            XYVector vec;
            vec.set_label(label);
            vec.setPosition(x1,y1);
            vec.setVectorXY(x2-x1,y2-y1);
//            if ( label.find("tulip",0) != std::string::npos ) {
                vec.setHeadSize(10);
//            } else {
//                vec.setHeadSize(10);
//            }

            label += "v";

            vec.color_set("red");

            std::string vec_s = vec.get_spec();

            ALogEntry new_entry;
            new_entry.set(
                    entry.getTimeStamp(),
                    "VIEW_VECTOR",
                    entry.getSource(),
                    entry.getSrcAux(),
                    vec_s);

            new_data.push_back(new_entry);
        } else if ( key == "VIEW_SEGLIST" && entry.getStringVal().find("label,")!=std::string::npos ) {
//            std::cout << entry.getStringVal() << std::endl;
            std::string msg = entry.getStringVal();
            MOOSChomp(msg,",");
            std::string label = MOOSChomp(msg,":") + "v";
//            std::cout << label << std::endl;

            MOOSChomp(msg,":");
            double x1 = atof(MOOSChomp(msg,",").c_str());
            double y1 = atof(MOOSChomp(msg,":").c_str());
            double x2 = atof(MOOSChomp(msg,",").c_str());
            double y2 = atof(MOOSChomp(msg,":").c_str());

            XYVector vec;
            vec.set_label(label);
            vec.setPosition(x1,y1);
            vec.setVectorXY(x2-x1,y2-y1);
//            if ( label.find("tulip",0) != std::string::npos ) {
//                vec.setHeadSize(10);
//            } else {
                vec.setHeadSize(0);
//            }

            label += "v";

            vec.color_set("yellow");

            std::string vec_s = vec.get_spec();

            ALogEntry new_entry;
            new_entry.set(
                    entry.getTimeStamp(),
                    "VIEW_VECTOR",
                    entry.getSource(),
                    entry.getSrcAux(),
                    vec_s);

            new_data.push_back(new_entry);

//            std::stringstream ss;
//            ss << new_entry.getTimeStamp() << "\t";
//            ss << new_entry.getVarName() << "\t";
//            ss << new_entry.getSource() << "\t";
//            ss << new_entry.getStringVal() << "\n";
//
//            fputs(ss.str().c_str(), logfile);

//            std::cout << vec_s << std::endl;
        }
        entry = JoshUtil::getNextRawALogEntry_josh(logfile);
    }

    fclose(logfile);

    std::ifstream infile;
    infile.open(argv[1]);

    std::ofstream outfile;
    outfile.open("out.alog");

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
        ss << new_entry.getStringVal() << "\n";

        outfile << ss.str();
    }

    infile.close();
    outfile.close();

	return 0;
}
