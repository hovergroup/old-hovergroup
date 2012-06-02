/*
 * acomms_alog_parser_main.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */



using namespace std;

#include <boost/filesystem.hpp>
#include <string>
#include "MBUtils.h"
#include <vector>

using namespace std;
namespace fs = boost::filesystem;

vector<fs::path> alog_files;

int main(int argc, char *argv[]) {
	bool recursive = false;
	if(scanArgs(argc, argv, "-R", "--recursive")) {
		recursive = true;
	}

	fs::path full_path( fs::initial_path<fs::path>() );

	if ( !recursive && argc > 1 )
		full_path = fs::system_complete( fs::path( argv[argc-1] ) );
	else if ( recursive && argc > 2 ) {
		full_path = fs::system_complete( fs::path( argv[argc-1] ) );
	} else {
		cout << "Must specify a directory." << endl;
		return 1;
	}

	cout << full_path << endl;

	if ( !fs::exists( full_path ) ) {
		cout << "Directory not found: " << full_path << endl;
		return 1;
	}

	if ( !fs::is_directory( full_path ) ) {
		cout << full_path << " is not a directory." << endl;
		return 1;
	}

	fs::directory_iterator end_iter;
	for ( fs::directory_iterator dir_itr( full_path );
			dir_itr != end_iter; ++dir_itr ) {

		try {
			if ( fs::is_directory( dir_itr->status() ) ) {
				fs::directory_iterator sub_dir( fs::system_complete( dir_itr->path().filename() ) );
				cout << "found directory: " << dir_itr->path().filename() << endl;
			} else if ( fs::is_regular_file( dir_itr->status() ) ) {
				cout << "found file: " << dir_itr->path().filename() << endl;
				cout << "extension: " << fs::extension( dir_itr->path().filename() ) << endl;
			}
		} catch ( const exception & ex ) {
			cout << dir_itr->path().filename() << " " << ex.what() << endl;
		}
	}


	return 0;
}

void searchDirectory ( fs::path directory_path ) {
	fs::directory_iterator end_iter;
	for ( fs::directory_iterator dir_itr( directory_path );
			dir_itr != end_iter; ++dir_itr ) {

		try {
			if ( fs::is_regular_file( dir_itr->status() ) ) {
				if ( fs::extension( dir_itr->path().filename() ) == ".alog" ) {
					alog_files.push_back( fs::system_complete( dir_itr->path().filename() ) );
				}
			}
		} catch ( const exception & ex ) {
			cout << dir_itr->path().filename() << " " << ex.what() << endl;
		}
	}
}
