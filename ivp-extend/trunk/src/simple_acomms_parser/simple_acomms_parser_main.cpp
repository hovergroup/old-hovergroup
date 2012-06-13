/*
 * acomms_alog_parser_main.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */



using namespace std;

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
#include <stdlib.h>


using namespace std;
using namespace boost::posix_time;
using namespace lib_acomms_messages;

#define MAX_LINE_LENGTH 10000

ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings = false)
{
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

int main(int argc, char *argv[]) {
	FILE *logfile = fopen( "log.alog", "r" );
	for ( int i=0; i<5; i++ ) {
		getNextRawLine( logfile );
	}

	ALogEntry entry = getNextRawALogEntry_josh( logfile );
	double gps_x, gps_y;

	ofstream output;
	output.open("log.txt");

	output << "gps_x, gps_y, snr_in, snr_out, spl" << endl;
	while ( entry.getStatus() != "eof" ) {
		string key = entry.getVarName();
		double msg_time = entry.getTimeStamp();
		if ( key == "GPS_X" || key == "NAV_X" ) {
			gps_x = entry.getDoubleVal();
		} else if ( key == "GPS_Y" || key == "NAV_X=Y" ) {
			gps_y = entry.getDoubleVal();
		} else if ( key == "ACOMMS_RECEIVED_ALL" ) {
			string msg_val = entry.getStringVal();

			goby::acomms::protobuf::ModemTransmission trans;
			string temp;
			while ( msg_val.find("<|>") != string::npos ) {
				msg_val.replace( msg_val.find("<|>"), 3, "\n" );
			}
			string const* ptr = &msg_val;
			google::protobuf::TextFormat::ParseFromString( *ptr, &trans );

			if ( trans.type() == goby::acomms::protobuf::ModemTransmission::DATA ) {
				int numstats = trans.ExtensionSize( micromodem::protobuf::receive_stat );
				micromodem::protobuf::ReceiveStatistics stat;
	//			if ( numstats == 2 )
	//				stat = trans.GetExtension( micromodem::protobuf::receive_stat, 1 );
				if ( numstats == 1 ) {
					stat = trans.GetExtension( micromodem::protobuf::receive_stat, 0 );

					output << gps_x << "," << gps_y << ",";
					output << stat.snr_in() << ",";
					output << stat.snr_out() << ",";
					output << stat.spl() << endl;
				}
			}
		}
		entry = getNextRawALogEntry_josh( logfile );
	}

	return 0;
}
