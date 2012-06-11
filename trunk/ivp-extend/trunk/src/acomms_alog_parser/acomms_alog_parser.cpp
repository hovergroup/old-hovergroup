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

	ALogEntry entry = getNextRawALogEntry( logfile );
	moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0 - entry.time();
}

bool ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaGPS() {
	ALogEntry entry = getNextRawALogEntry( logfile );
	if ( entry.getStatus() == "invalid" )
		return false;
	double first_moos_time = entry.getTimeStamp();
	int first_moos_time_seconds = floor(entry.getTimeStamp());
	int first_moos_time_milliseconds = floor(entry.getTimeStamp()*1000.0) - first_moos_time_seconds*1000;
	time_duration first_moos_time_td = seconds( first_moos_time_seconds ) +
			milliseconds( first_moos_time_milliseconds );
	while ( entry.getStatus() != "eof" ) {
		if ( entry.getVarName() == "GPS_PTIME" ) {
			cout << entry.getStringVal() << endl;
			ptime first_gps_ptime( time_from_string( entry.getStringVal() ) );
			int first_gps_moos_time_seconds = floor(entry.getTimeStamp() );
			int first_gps_moos_time_milliseconds = floor(entry.getTimeStamp()*1000.0) - first_gps_moos_time_seconds*1000;
			time_duration first_gps_td = seconds ( first_gps_moos_time_seconds ) +
					milliseconds( first_gps_moos_time_milliseconds );

			creation_time = first_gps_ptime - first_gps_td - first_moos_time_td;
			moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0 -first_moos_time;
			return true;
		}
		entry = getNextRawALogEntry( logfile );
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
		cout << alog_files[i].filename << "  header offset: " << alog_files[i].moos_time_offset <<
				"  creation time: " << to_simple_string(alog_files[i].creation_time) << endl;
//		cout << alog_files[i].filename << "  " << alog_files[i].header_lines[1] << endl;
	}
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

void ACOMMS_ALOG_PARSER::generateHistories() {
	for ( int i=0; i<alog_files.size(); i++ ) {
		vehicle_histories[alog_files[i].vehicle_name].vehicle_logs.push_back(alog_files[i]);
	}
	for ( int i=0; i<vehicle_histories.size(); i++ ) {

	}
}
