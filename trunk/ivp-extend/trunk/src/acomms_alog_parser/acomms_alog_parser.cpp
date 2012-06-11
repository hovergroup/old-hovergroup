/*
 * acomms_alog_parser.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */

#include "acomms_alog_parser.h"

using namespace std;
using namespace boost::posix_time;

ACOMMS_ALOG_PARSER::ACOMMS_ALOG_PARSER() {
}

void ACOMMS_ALOG_PARSER::runParser() {
	parseAllHeaders();
	parseMOOSFiles();
	generateHistories();
}

void ACOMMS_ALOG_PARSER::addAlogFile( std::string filename ) {
	FILE_INFO new_info;
	new_info.filename = filename;
	new_info.logfile = fopen( filename.c_str(), "r" );
	alog_files.push_back( new_info );
}

void ACOMMS_ALOG_PARSER::addAlogFile( boost::filesystem::path filepath ) {
	FILE_INFO new_info;
	new_info.filename = string(filepath.string().c_str());
	new_info.logfile = fopen( filepath.string().c_str(), "r" );
	alog_files.push_back( new_info );
}

string ACOMMS_ALOG_PARSER::FILE_INFO::getNextLine() {
	return getNextRawLine( logfile );
}

void ACOMMS_ALOG_PARSER::FILE_INFO::parseHeaderLines() {
	getNextLine();
	for ( int i=0; i<3; i++ )
		header_lines.push_back( getNextLine() );
	getNextLine();
}

void ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaHeader() {
	int date_start = header_lines[1].find("ON ") + 3;
	string date_string = header_lines[1].substr(date_start, header_lines[1].size()-date_start);
	if ( date_string[0] == ' ' )
		date_string.erase(date_string.begin());
//	cout << date_string << endl;
	tm header_time;
	strptime( date_string.c_str(), "%a %b %d %H:%M:%S %Y", &header_time);
//	cout << asctime( &header_time ) << endl;
	creation_time = boost::posix_time::ptime_from_tm( header_time );
//	cout << boost::posix_time::to_simple_string( creation_time ) << endl;

//	ALogEntry entry = getNextRawALogEntry( logfile );
	moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0;// - entry.time();
}

bool ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaGPS() {
	ALogEntry entry = getNextRawALogEntry_josh( logfile );
	if ( entry.getStatus() == "invalid" )
		return false;
//	double first_moos_time = entry.getTimeStamp();
//	int first_moos_time_seconds = floor(entry.getTimeStamp());
//	int first_moos_time_milliseconds = floor(entry.getTimeStamp()*1000.0) - first_moos_time_seconds*1000;
//	time_duration first_moos_time_td = seconds( first_moos_time_seconds ) +
//			milliseconds( first_moos_time_milliseconds );
//
//	cout << "first moos time " << first_moos_time_td << endl;
	while ( entry.getStatus() != "eof" ) {
		if ( entry.getVarName() == "GPS_PTIME" ) {
//			cout << ">>" << entry.getStringVal()<< "<<" << endl;
			ptime first_gps_ptime;
			try {
				first_gps_ptime = time_from_string( entry.getStringVal() );
			} catch ( exception &e ) {
				return false;
			}
			time_duration utc_correction = hours(UTC_TIME_OFFSET);
			first_gps_ptime += utc_correction;
			int first_gps_moos_time_seconds = floor(entry.getTimeStamp() );
			int first_gps_moos_time_milliseconds = floor(entry.getTimeStamp()*1000.0) - first_gps_moos_time_seconds*1000;
			time_duration first_gps_td = seconds ( first_gps_moos_time_seconds ) +
					milliseconds( first_gps_moos_time_milliseconds );

//			cout << "first gps time " << first_gps_td << endl;
//			cout << "ptime " << first_gps_ptime << endl;

			creation_time = first_gps_ptime - first_gps_td;// - first_moos_time_td;
			moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0;// -first_moos_time;
			return true;
		}
		entry = getNextRawALogEntry_josh( logfile );
	}
	return false;
}

void ACOMMS_ALOG_PARSER::parseAllHeaders() {
	for ( int i=0; i<alog_files.size(); i++ ) {
		alog_files[i].parseHeaderLines();
		if ( !alog_files[i].offsetViaGPS() ) {
			cout << "Failed to offset using gps time, using header." << endl;
			alog_files[i].offsetViaHeader();
		}
//		cout << alog_files[i].filename << "  moos time offset: " << alog_files[i].moos_time_offset <<
//				"  creation time: " << to_simple_string(alog_files[i].creation_time) << endl;
	}
}

void ACOMMS_ALOG_PARSER::FILE_INFO::resetFile() {
	fclose( logfile );
	logfile = fopen( filename.c_str(), "r");
	parseHeaderLines();
}

void ACOMMS_ALOG_PARSER::FILE_INFO::parseMOOSFile() {
	string moosfilename = filename;
	int extension_index = moosfilename.find(".alog");
	moosfilename.replace(extension_index, 5, "._moos");

	FILE * this_moos_file = fopen( moosfilename.c_str(), "r");
	string line = getNextRawLine( this_moos_file );
	while ( line != "eof" ) {
		if ( line.find("community") != string::npos || line.find("Community") != string::npos ) {
			int tmp_index = line.find("=")+1;
			vehicle_name = stripBlankEnds( line.substr(tmp_index, line.size()-tmp_index) );
			break;
		}
		line = getNextRawLine( this_moos_file );
	}

	fclose( this_moos_file );

	CProcessConfigReader missionreader;
	missionreader.SetFile( moosfilename );
	missionreader.GetConfigurationParam("iacomms_driver", "ID", vehicle_id);

//	cout << "filename: " << filename <<
//			"  vehicle: " << vehicle_name <<
//			"  id: " << vehicle_id << endl;
}

void ACOMMS_ALOG_PARSER::parseMOOSFiles() {
	for ( int i=0; i<alog_files.size(); i++ ) {
		alog_files[i].parseMOOSFile();
	}
}

bool history_sort ( ACOMMS_ALOG_PARSER::FILE_INFO f1, ACOMMS_ALOG_PARSER::FILE_INFO f2 ) {
	return f1.moos_time_offset < f2.moos_time_offset;
}

void ACOMMS_ALOG_PARSER::generateHistories() {
	for ( int i=0; i<alog_files.size(); i++ ) {
//		cout << alog_files[i].vehicle_name << endl;
		vehicle_histories[alog_files[i].vehicle_name].vehicle_logs.push_back(alog_files[i]);
		alog_files[i].resetFile();
		if ( find( vehicle_names.begin(), vehicle_names.end(), alog_files[i].vehicle_name ) == vehicle_names.end() ) {
			vehicle_names.push_back( alog_files[i].vehicle_name );
		}
	}
	for ( int i=0; i<vehicle_names.size(); i++ ) {
		string vehicle_name = vehicle_names[i];
		cout << endl << vehicle_name << endl;
		sort( 	vehicle_histories[vehicle_name].vehicle_logs.begin(),
				vehicle_histories[vehicle_name].vehicle_logs.end(),
				history_sort );
		for ( int j=0; j<vehicle_histories[vehicle_name].vehicle_logs.size(); j++ ) {
			cout << vehicle_histories[vehicle_name].vehicle_logs[j].filename << endl;
		}
	}
}

ALogEntry ACOMMS_ALOG_PARSER::getNextRawALogEntry_josh(FILE *fileptr, bool allstrings)
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
