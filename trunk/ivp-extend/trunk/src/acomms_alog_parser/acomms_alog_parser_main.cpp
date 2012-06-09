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

void searchDirectory ( fs::path directory_path, int depth );

int main(int argc, char *argv[]) {
	fs::path full_path( fs::initial_path<fs::path>() );

	if ( argc > 1 ) {
		full_path = fs::system_complete( fs::path( argv[argc-1] ) );
	} else {
		cout << "Must specify a directory." << endl;
		return 1;
	}

	if ( !fs::exists( full_path ) ) {
		cout << "Directory not found: " << full_path << endl;
		return 1;
	}

	if ( !fs::is_directory( full_path ) ) {
		cout << full_path << " is not a directory." << endl;
		return 1;
	}

	searchDirectory ( full_path, 0 );

	ACOMMS_ALOG_PARSER parser;
	cout << "Found " << alog_files.size() << " alog files." << endl;
	for ( int i=0; i<alog_files.size(); i++ ) {
//		cout << alog_files[i] << endl;
		parser.addAlogFile( alog_files[i] );
	}

	parser.runParser();

	return 0;
}

void searchDirectory ( fs::path directory_path, int depth ) {
	if ( depth > 2 ) return;

	fs::directory_iterator end_iter;
	for ( fs::directory_iterator dir_itr( directory_path );
			dir_itr != end_iter; ++dir_itr ) {

		try {
			if ( fs::is_regular_file( dir_itr->status() ) ) {
				if ( fs::extension( dir_itr->path().filename() ) == ".alog" ) {
					alog_files.push_back( fs::system_complete( dir_itr->path() ) );
				}
			} else if ( fs::is_directory( dir_itr->status() ) )
				searchDirectory( fs::system_complete( dir_itr->path() ), depth+1 );
		} catch ( const exception & ex ) {
			cout << dir_itr->path().filename() << " " << ex.what() << endl;
		}
	}
}
