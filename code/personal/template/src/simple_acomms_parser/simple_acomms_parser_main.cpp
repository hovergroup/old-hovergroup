/*
 * simple_acomms_parser
 *        File: simple_acomms_parser_main.cpp
 *  Created on: Jun 13, 2012
 *      Author: Josh Leighton
 */

// copy to your own tree before making modifications
// this version is subject to being changed or overwritten
using namespace std;

#include "goby/acomms/protobuf/mm_driver.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <google/protobuf/text_format.h>
#include "LogUtils.h"
#include "MBUtils.h"
#include "MOOS/libMOOS/Utils/ProcessConfigReader.h"
#include <stdio.h>
#include <vector>
#include <time.h>
#include <map>
#include <math.h>
#include <algorithm>
#include <stdlib.h>

using namespace std;
using namespace boost::posix_time;

#define MAX_LINE_LENGTH 10000

ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings = false);
void printHelp();

ofstream output;

double last_print_time = -100;

// define the variables you want to keep track of here
double gps_x, gps_y, sensor_heading, current_heading, desired_heading;
double nav_x, nav_y;
//int snr_in, snr_out, spl, stddev_noise, mse_equalizer;

void printLine(double time_stamp) {
    output << time_stamp << ",";
    output << gps_x << ",";
    output << gps_y << ",";
    output << sensor_heading << ",";
    output << current_heading << ",";
    output << desired_heading << ",";
    output << nav_x << ",";
    output << nav_y << endl;
    //output << snr_in << ",";
    //output << snr_out << ",";
    //output << spl << ",";
    //output << stddev_noise << ",";
    //output << mse_equalizer << endl;
}

void printHeader() {
    //output << "gps_x, gps_y, snr_in, snr_out, spl, noise, mse" << endl;
    output << "time_stamp, gps_x, gps_y, sensor_heading, current_heading"
            << endl;
}

int main(int argc, char *argv[]) {
    // Look for a request for version information
    if (scanArgs(argc, argv, "-h", "--help")) {
        printHelp();
        return (0);
    }

    if (argc != 2) {
        cout << "Insufficient arguments - must provide input log file." << endl;
        return 0;
    }
    FILE *logfile = fopen(argv[1], "r");
    for (int i = 0; i < 5; i++) {
        getNextRawLine(logfile);
    }

    // open output file and print header
    output.open("log.txt");
    printHeader();

    // read first alog entry
    ALogEntry entry = getNextRawALogEntry_josh(logfile);
    while (entry.getStatus() != "eof") {
        // variable name and message time
        string key = entry.getVarName();
        double msg_time = entry.getTimeStamp();

        if (key == "GPS_X") {
            gps_x = entry.getDoubleVal();
        } else if (key == "GPS_Y") {
            gps_y = entry.getDoubleVal();
        } else if (key == "COMPASS_HEADING_FILTERED") {
            sensor_heading = entry.getDoubleVal();
        } else if (key == "NAV_HEADING") {
            current_heading = entry.getDoubleVal();
            if (entry.getSource() != "pEchoVar_a") {
                printLine(msg_time);
            }
        } else if (key == "NAV_X") {
            nav_x = entry.getDoubleVal();
        } else if (key == "NAV_Y") {
            nav_y = entry.getDoubleVal();
        } else if (key == "DESIRED_HEADING") {
            desired_heading = entry.getDoubleVal();
        }

        // parsing acomms statistics data
        else if (key == "ACOMMS_RECEIVED_ALL") {
            string msg_val = entry.getStringVal();

            goby::acomms::protobuf::ModemTransmission trans;
            string temp;
            while (msg_val.find("<|>") != string::npos) {
                msg_val.replace(msg_val.find("<|>"), 3, "\n");
            }
            string const* ptr = &msg_val;
            google::protobuf::TextFormat::ParseFromString(*ptr, &trans);

            if (trans.type()
                    == goby::acomms::protobuf::ModemTransmission::DATA) {
                int numstats = trans.ExtensionSize(
                        micromodem::protobuf::receive_stat);
                micromodem::protobuf::ReceiveStatistics stat;
                //			if ( numstats == 2 )
                //				stat = trans.GetExtension( micromodem::protobuf::receive_stat, 1 );
                if (numstats == 1) {
                    stat = trans.GetExtension(
                            micromodem::protobuf::receive_stat, 0);

                    //					snr_in = stat.snr_in();
                    //					snr_out = stat.snr_out();
                    //					spl = stat.spl();
                    //					stddev_noise = stat.stddev_noise();
                    //					mse_equalizer = stat.mse_equalizer();

                    // here we print a line whenever we get an acomms reception
//				printLine( msg_time );
                }
            }
        }

        // here we print a line every .1 seconds
        //		if ( msg_time - last_print_time > .1 ) {
        //			printLine( msg_time );
        //			last_print_time = msg_time;
        //		}

        // get the next entry from the log
        entry = getNextRawALogEntry_josh(logfile);
    }

    return 0;
}

// nothing past here to worry about

void printHelp() {

}

ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings) {
    ALogEntry entry;
    if (!fileptr) {
        cout << "failed getNextRawALogEntry() - null file pointer" << endl;
        entry.setStatus("invalid");
        return (entry);
    }

    bool EOLine = false;
    bool EOFile = false;
    int buffix = 0;
    int lineix = 0;
    int myint = '\0';
    char buff[MAX_LINE_LENGTH];

    string time, var, rawsrc, val;

    // Simple state machine:
    //   0: time
    //   1: between time and variable
    //   2: variable
    //   3: between variable and source
    //   4: source
    //   5: between source and value
    //   6: value
    int state = 0;

    while ((!EOLine) && (!EOFile) && (lineix < MAX_LINE_LENGTH)) {
        //		cout << "state: " << state << endl;
        myint = fgetc(fileptr);
        unsigned char mychar = myint;
        switch (myint) {
        case EOF:
            EOFile = true;
            break;
        case ' ':
            if (state == 6) {
                buff[buffix] = mychar;
                buffix++;
            }
            //			break;
        case '\t':
            if (state == 0) {
                buff[buffix] = '\0';
                time = buff;
                buffix = 0;
                state = 1;
            } else if (state == 2) {
                buff[buffix] = '\0';
                var = buff;
                buffix = 0;
                state = 3;
            } else if (state == 4) {
                buff[buffix] = '\0';
                rawsrc = buff;
                buffix = 0;
                state = 5;
            }
            break;
        case '\n':
            buff[buffix] = '\0'; // attach terminating NULL
            val = buff;
            EOLine = true;
            break;
        default:
            if (state == 1)
                state = 2;
            else if (state == 3)
                state = 4;
            else if (state == 5)
                state = 6;
            buff[buffix] = mychar;
            buffix++;
        }
        lineix++;
    }

    string src = biteString(rawsrc, ':');
    string srcaux = rawsrc;

    val = stripBlankEnds(val);

    //	cout << "t:" << time << " v:" << var << " s:" << src << " v:" << val << endl;

    if ((time != "") && (var != "") && (src != "") && (val != "")
            && isNumber(time)) {
        if (allstrings || !isNumber(val))
            entry.set(atof(time.c_str()), var, src, srcaux, val);
        else
            entry.set(atof(time.c_str()), var, src, srcaux, atof(val.c_str()));
    } else {
        if (EOFile)
            entry.setStatus("eof");
        else
            entry.setStatus("invalid");
    }

    return (entry);
}
