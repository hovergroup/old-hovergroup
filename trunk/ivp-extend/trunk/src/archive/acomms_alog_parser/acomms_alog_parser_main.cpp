/*
 * acomms_alog_parser_main.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */



using namespace std;

#include "acomms_alog_parser.h"
#include <string>
#include "MBUtils.h"

using namespace std;
namespace fs = boost::filesystem;

vector<fs::path> alog_files;

struct CONFIG {
	std::vector<std::string> string_variables;
	std::vector<std::string> double_variables;
};

void searchDirectory ( fs::path directory_path, int depth );
struct CONFIG readConfigFromFile( FILE * config_file );
bool promptYesNo( bool defaultValue ) {
	while (1) {
		char response;
		cin.get( response );
		if ( response=='y' || response=='Y' )
			return true;
		else if ( response =='n' || response=='N' )
			return false;
		else if ( response == '\n' )
			return defaultValue;
		cin.get();
	}
}

int main(int argc, char *argv[]) {
	ACOMMS_ALOG_PARSER parser;
	CONFIG my_config;

	// look for config file in our current directory
	FILE * config_file;
	config_file = fopen("parser.conf", "r");
	if ( config_file == NULL) {
		cout << "No config file found, proceed anyways? (Y/n) ";
		if ( !promptYesNo(true) )
			return 0;
	} else {
		my_config = readConfigFromFile( config_file );
		fclose( config_file );
		// pass parser variable names
		for ( int i=0; i<my_config.double_variables.size(); i++ ) {
			parser.addDoubleVar( my_config.double_variables[i] );
		}
		for ( int i=0; i<my_config.string_variables.size(); i++ ) {
			parser.addStringVar( my_config.string_variables[i] );
		}
	}

	// define the path where we look for alog files
	fs::path full_path( fs::initial_path<fs::path>() );
	if ( argc > 1 ) {
		full_path = fs::system_complete( fs::path( argv[argc-1] ) );
	} else {
		cout << "Must specify a directory." << endl;
		return 1;
	}
	// check that specified folder exists
	if ( !fs::exists( full_path ) ) {
		cout << "Directory not found: " << full_path << endl;
		return 1;
	}
	// check that specified path is a directory and not a file
	if ( !fs::is_directory( full_path ) ) {
		cout << full_path << " is not a directory." << endl;
		return 1;
	}

	// look for alog files in the specified directory
	searchDirectory ( full_path, 0 );

	// pass parser alog files
	cout << "Found " << alog_files.size() << " alog files." << endl;
	for ( int i=0; i<alog_files.size(); i++ ) {
//		cout << alog_files[i] << endl;
		parser.addAlogFile( alog_files[i] );
	}

	// run the parser
	parser.runParser();

	return 0;
}

void searchDirectory ( fs::path directory_path, int depth ) {
	if ( depth > 2 ) return; // maximum search depth

	// iterate over items in the directory
	fs::directory_iterator end_iter;
	for ( fs::directory_iterator dir_itr( directory_path );
			dir_itr != end_iter; ++dir_itr ) {

		try {
			// if file, check file extension
			if ( fs::is_regular_file( dir_itr->status() ) ) {
				if ( fs::extension( dir_itr->path().filename() ) == ".alog" ) {
					alog_files.push_back( fs::system_complete( dir_itr->path() ) );
				}
			// if directory, search in that directory after incrementing depth
			} else if ( fs::is_directory( dir_itr->status() ) )
				searchDirectory( fs::system_complete( dir_itr->path() ), depth+1 );
		} catch ( const exception & ex ) {
			cout << dir_itr->path().filename() << " " << ex.what() << endl;
		}
	}
}

struct CONFIG_LINE {
	string status;
	// invalid, eof, or empty

	string first;
	string second;
};

struct CONFIG_LINE readConfigLine( FILE * config_file ) {
	bool EOLine = false, EOFile = false;
	int buffix = 0;
	int lineix = 0;
	char buff[200];
	int myint = '\0';
	CONFIG_LINE result;

	while ( (!EOLine) && (!EOFile) && (lineix < 200) ) {
		myint = fgetc( config_file );
		unsigned char mychar = myint;
		switch ( myint ) {
		case EOF:
			EOFile = true;
			break;
		case ' ':
			break;
		case '\t':
			break;
		case '\n':
			buff[buffix]='\0';
			result.second = buff;
			EOLine = true;
			break;
		case '=':
			buff[buffix]='\0';
			result.first = buff;
			buffix = 0;
			break;
		default:
			buff[buffix] = mychar;
			buffix++;
		}
		lineix++;
	}
	if ( EOFile ) {
		result.status = "eof";
	} else if ( result.first.empty() && result.second.empty() ) {
		result.status = "empty";
	} else if ( result.first.empty() || result.second.empty() ) {
		result.status = "invalid";
	} else {
		result.status = "okay";
	}
	return result;
}

struct CONFIG readConfigFromFile( FILE * config_file ) {
	CONFIG my_config;
	CONFIG_LINE next_line = readConfigLine( config_file );
	int line_num = 1;
	while ( next_line.status != "eof" ) {
		if ( next_line.status == "invalid" ) {
			cout << "Error reading config file at line " << line_num << endl;
			exit(1);
		} else if ( next_line.status == "okay" ) {
			if ( next_line.first=="STRING_VAR" || next_line.first=="string_var" ) {
				my_config.string_variables.push_back( next_line.second );
			} else if ( next_line.first=="DOUBLE_VAR" || next_line.first=="double_var" ) {
				my_config.double_variables.push_back( next_line.second );
			} else {
				cout << "Error reading config file at line " << line_num << endl;
				exit(1);
			}
		}
		next_line = readConfigLine( config_file );
		line_num++;
	}
	return my_config;
}
