/*
 * acomms_alog_parser.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */

#include "acomms_alog_parser.h"

using namespace std;

ACOMMS_ALOG_PARSER::ACOMMS_ALOG_PARSER() {
}

void ACOMMS_ALOG_PARSER::runParser() {
	parseAllHeaders();
}

void ACOMMS_ALOG_PARSER::addAlogFile( std::string filename ) {
	FILE_INFO new_info;
	new_info.filename = filename;
	new_info.logfile = fopen( filename.c_str(), "r" );
	alog_files.push_back( new_info );
}

void ACOMMS_ALOG_PARSER::addAlogFile( boost::filesystem::path filepath ) {
	FILE_INFO new_info;
	new_info.filename = string(filepath.c_str());
	new_info.logfile = fopen( filepath.c_str(), "r" );
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
}

void ACOMMS_ALOG_PARSER::parseAllHeaders() {
	for ( int i=0; i<alog_files.size(); i++ ) {
		alog_files[i].parseHeaderLines();
//		cout << alog_files[i].filename << "  " << alog_files[i].header_lines[1] << endl;
	}
}
