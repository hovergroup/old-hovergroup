/*
 * JoshUtil.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: josh
 */

#include "JoshUtils.h"

#define MAX_LINE_LENGTH 10000

void subSearchForFiles( std::vector<std::string> & paths,
			boost::filesystem::path directory_path,
			int max_depth,
			std::string wild,
			int current_depth ) {
	if (current_depth > max_depth)
		return; // maximum search depth

	// iterate over items in the directory
	boost::filesystem::directory_iterator end_iter;
	for (boost::filesystem::directory_iterator dir_itr(directory_path);
			dir_itr != end_iter; ++dir_itr) {

		try {
			// if file, check file extension
			if (boost::filesystem::is_regular_file(dir_itr->status())) {
				if (JoshUtil::wildCardMatch(wild, dir_itr->path().filename().string())) {
					std::string this_path = boost::filesystem::system_complete(dir_itr->path()).string();
					if (std::find(paths.begin(),paths.end(),this_path)==
							(std::vector<std::string>::const_iterator) paths.end() )
						paths.push_back(this_path);
				}
				// if directory, search in that directory after incrementing depth
			} else if (boost::filesystem::is_directory(dir_itr->status()))
				subSearchForFiles(paths,
						boost::filesystem::system_complete(dir_itr->path()),
						max_depth, wild, current_depth+1);
		} catch (const std::exception & ex) {
			std::cout << dir_itr->path().filename() << " " << ex.what()
					<< std::endl;
		}
	}
}

ALogEntry JoshUtil::getNextRawALogEntry_josh(FILE *fileptr, bool allstrings) {
	ALogEntry entry;
	if (!fileptr) {
		std::cout << "failed getNextRawALogEntry() - null file pointer" << std::endl;
		entry.setStatus("invalid");
		return (entry);
	}

	bool EOLine = false;
	bool EOFile = false;
	int buffix = 0;
	int lineix = 0;
	int myint = '\0';
	char buff[MAX_LINE_LENGTH];

	std::string time, var, rawsrc, val;

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

	std::string src = biteString(rawsrc, ':');
	std::string srcaux = rawsrc;

	val = stripBlankEnds(val);

	//	cout << "t:" << time << " v:" << var << " s:" << src << " v:" << val << endl;
	if (((time != "") && (var != "") && (src != "") && isNumber(time)) &&
			( allstrings || !isNumber(val) ) ) {
		entry.set(atof(time.c_str()), var, src, srcaux, val);
	} else if ((time != "") && (var != "") && (src != "") && (val != "")
			&& isNumber(time)) {
		entry.set(atof(time.c_str()), var, src, srcaux, atof(val.c_str()));
	} else {
		if (EOFile)
			entry.setStatus("eof");
		else
			entry.setStatus("invalid");
	}

	return (entry);
}

/**
 * returns true if key matches pattern wild ( * for wildcards )
 */
bool JoshUtil::wildCardMatch( std::string wild, std::string key ) {
	if ( wild == key ) return true;
	if ( wild.empty() || key.empty() ) return false;
	if ( wild.find("*") == std::string::npos ) return false;

	int wild_position = wild.find("*");
	if ( wild_position == 0 ) {
		std::string post_wild = wild.substr(1, wild.size()-1);
		if ( key.size() < post_wild.size() ) return false;
		std::string key_end = key.substr( key.size()-post_wild.size(), post_wild.size() );
		if ( post_wild == key_end ) return true;
		else return false;
	} else if ( wild_position == wild.size()-1 ) {
		std::string pre_wild = wild.substr(0,wild_position);
		if ( key.size() < pre_wild.size() ) return false;
		std::string key_start = key.substr( 0, pre_wild.size() );
		if ( pre_wild == key_start ) return true;
		else return false;
	} else {
		std::string pre_wild = wild.substr(0,wild_position);
		std::string post_wild = wild.substr(wild_position+1, wild.size()-wild_position-1);
		if ( key.size() < pre_wild.size() + post_wild.size() ) return false;
		std::string key_begin = key.substr( 0, pre_wild.size() );
		std::string key_end = key.substr( key.size()-post_wild.size(), post_wild.size() );
		if ( key_begin==pre_wild && key_end==post_wild ) return true;
		else return false;
	}
}

/**
 * searches directories for files matching wildcard
 * will search in sub folders up to max_depth, starting at 0
 */
void JoshUtil::searchForFiles(std::vector<std::string> & paths,
		std::string directory_path, int max_depth, std::string wild) {
	subSearchForFiles(paths,
			boost::filesystem::system_complete(boost::filesystem::path(directory_path)),
			max_depth, wild, 0);
}
