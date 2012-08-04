/*
 * alogParse.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: josh
 */

#include "alogParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <math.h>

#define MAX_LINE_LENGTH 10000
ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings = false);
std::string readCommandArg( std::string sarg );
bool wildCardMatch( std::string wild, std::string key );

// functions copied from moos-ivp to support independent compilation
std::string biteString(std::string& str, char separator);
std::string stripBlankEnds(const std::string& str);
bool isNumber(const std::string& str, bool blanks_allowed=true);
bool scanArgs(int, char**, const char*, const char *a=0, const char *b=0);
bool strContains(const std::string& str, const std::string& qstr);

using namespace std;

map<string,vector<pair<double,string> > > values;
vector<string> variables, wilds;
ofstream output;
vector<int> startIndices;

void printHelp() {
    cout << "Usage: " << endl;
    cout << "  alogParse in.alog --sync_variable=[SYNC_VAR] [VAR] [OPTIONS]   " << endl;
    cout << "  alogParse in.alog --sync_period=[period] [VAR] [OPTIONS]       " << endl;
    cout << "                                                                 " << endl;
    cout << "Synopsis:                                                        " << endl;
    cout << "  Creates a comma delimited synchronous log file from an alog    " << endl;
    cout << "  file for importing into Matlab, Excel, or similar.  Age of each" << endl;
    cout << "  variable is also reported (may be negative).                   " << endl;
    cout << "                                                                 " << endl;
    cout << "Standard Arguments:                                              " << endl;
    cout << "  in.alog  - The input logfile.                                  " << endl;
    cout << "  VAR      - The name of a MOOS variable                         " << endl;
    cout << "                                                                 " << endl;
    cout << "Synchronization Options:                                         " << endl;
    cout << "  Sync by variable  - a line will be printed everytime the       " << endl;
    cout << "                      variable [SYNC_VAR] is posted to.          " << endl;
    cout << "  Sync periodically - a line will be printed every [period]      " << endl;
    cout << "                      seconds.                                   " << endl;
    cout << "                                                                 " << endl;
    cout << "Options:                                                         " << endl;
    cout << "  -h,--help        Displays this help message                    " << endl;
    cout << "  -b,--backsearch  When printing a line, look only backards for  " << endl;
    cout << "                   the latest posting of each variable. (Age will" << endl;
    cout << "                   always be positive).                          " << endl;
    cout << "                                                                 " << endl;
    cout << "Further Notes:                                                   " << endl;
    cout << "  (1) The output file will have the same filename as the input,  " << endl;
    cout << "      but with a .txt extension.                                 " << endl;
    cout << "  (2) Use * for wildcard logging (e.g. VAR* matches any MOOS     " << endl;
    cout << "      variable starting with VAR)                                " << endl;
    cout << endl;
}


bool general_sort( pair<double,string> obj1, pair<double,string> obj2 ) {
	return obj1.first < obj2.first;
}

// find the nearest entry by time, returns index and time diff
pair<int,double> findNearest( vector< pair<double,string> > item_list, double msg_time,
		int startindex ) {
	if ( startindex < 0 ) startindex = 0;
	if ( item_list.empty() )
		return pair<int,double>(-1,-1);
	double min_diff = 10000;
	int index = -1;
	for ( int i=startindex; i<item_list.size(); i++ ) {
		if ( fabs(item_list[i].first-msg_time) < min_diff ) {
			min_diff = fabs(item_list[i].first-msg_time);
			index = i;
		}
		if ( item_list[i].first > msg_time )
			break;
	}
//	cout << index-startindex << "  searched from " << startindex << " to " << index << endl;
	double age = msg_time - item_list[index].first;
	return pair<int,double>(index,age);
}

// find the latest entry by time, returns index and time diff
pair<int,double> findLatest( vector< pair<double,string> > item_list, double msg_time,
		int startindex ) {
	if ( startindex < 0 ) startindex = 0;
	if ( item_list.empty() || item_list.front().first > msg_time )
		return pair<int,double>(-1,-1);

	int index = startindex;
	while ( item_list[index].first <= msg_time && index < item_list.size() ) {
		index++;
	}
	if ( index==0 )
		index++;
	double age = msg_time-item_list[index-1].first;
	return pair<int,double>(index-1,age);
}

void printHeader() {
	output << "time";
	for ( int i=0; i<variables.size(); i++ ) {
		output << "," << variables[i] << "," << variables[i] << "_age";
	}
	output << endl;
}

// print a line of data for a certain time
void printTime( double msg_time, bool backsearch_only ) {
	output << msg_time;
	for ( int i=0; i<variables.size(); i++ ) {
		string var = variables[i];
		pair<int,double> result;
		if ( !backsearch_only )
			result = findNearest(values[var], msg_time, startIndices[i]);
		else
			result = findLatest(values[var], msg_time, startIndices[i]);
		if ( result.first == -1 )
			output << ",-1,-1";
		else
			output << "," << values[var][result.first].second << "," << result.second;
		startIndices[i] = result.first;
	}
	output << endl;
}

int main (	int argc, char *argv[] ) {
	string sync_variable;
	bool use_sync_variable = false;
	double sync_period = 0;
	bool restrict_to_backsearch = false;

	if( scanArgs(argc, argv, "-h", "--help" ) ) {
		printHelp();
		return 0;
	}

	// read command line arguments
	string alogfile_in;
	for (int i = 1; i < argc; i++) {
		string sarg = argv[i];
		if (strContains(sarg, ".alog")) {
			if (alogfile_in == "")
				alogfile_in = sarg;
		} else if ( strContains(sarg, "--sync_variable=" ) ) {
			use_sync_variable = true;
			sync_variable = readCommandArg( string(sarg) );
		} else if ( strContains(sarg, "--sync_period=" ) ) {
			sync_period = atof( readCommandArg(string(sarg)).c_str() );
		} else if ( sarg == "-b" || sarg == "--backsearch" ) {
			restrict_to_backsearch = true;
		} else if ( sarg.find("*")!=string::npos ) {
			wilds.push_back( sarg );
		} else {
			variables.push_back( sarg );
		}
	}

	// check if sync variable matches a wildcard
	if ( use_sync_variable && !wilds.empty()) {
		for (int i=0; i<wilds.size(); i++ ) {
			if ( wildCardMatch(wilds[i],sync_variable) ) {
				variables.push_back(sync_variable);
				break;
			}
		}
	}

	if (alogfile_in == "") {
		cout << "No alog file given - exiting" << endl;
		exit(0);
	}
	if ( !use_sync_variable && sync_period==0 ) {
		cout << "No sync period or variable set - exiting" << endl;
		exit(0);
	}
	if ( use_sync_variable && sync_variable=="" ) {
		cout << "No sync variable set - exiting" << endl;
		exit(0);
	}

	FILE *logfile = fopen( alogfile_in.c_str(), "r" );
	if ( logfile == NULL ) {
		cout << "Alog file does not exist - exiting" << endl;
		exit(0);
	}

	double start_time = -100;
	double stop_time = -100;
	// read through alog file, picking out variables we care about
	int line_num = 1;
	ALogEntry entry = getNextRawALogEntry_josh( logfile );
	while( entry.getStatus() != "eof" ) {
		line_num++;
		if ( entry.getStatus() == "okay" ) {
//			cout << entry.getVarName() << endl;
			string key = entry.getVarName();
			double msg_time = entry.getTimeStamp();
			if ( find(variables.begin(), variables.end(), key) != variables.end() ||
					key == sync_variable ) {
				values[key].push_back( pair<double,string>( msg_time, entry.getStringVal() ) );
				// these start and stop times will be used for period synchronization
				if ( start_time == -100 )
					start_time = msg_time;
				if ( msg_time > stop_time )
					stop_time = msg_time;
			} else {
				for ( int i=0; i<wilds.size(); i++ ) {
					if ( wildCardMatch(wilds[i], key) ) {
						variables.push_back(key);
						values[key].push_back( pair<double,string>( msg_time, entry.getStringVal() ) );
						// these start and stop times will be used for period synchronization
						if ( start_time == -100 )
							start_time = msg_time;
						if ( msg_time > stop_time )
							stop_time = msg_time;
						break;
					}
				}
			}
		}
		entry = getNextRawALogEntry_josh( logfile );
	}

	cout << "Read " << line_num << " lines from log file." << endl;

	for ( int i=0; i<variables.size(); i++ ) {
		sort( values[variables[i]].begin(), values[variables[i]].end(), general_sort);
	}

	// open output file and print header
	string file_out = alogfile_in;
	file_out.replace( file_out.size()-4, 4, "txt");
	output.open(file_out.c_str());
	printHeader();

	startIndices = vector<int>(variables.size(), -1);
	if ( use_sync_variable ) {
		// print a line everytime sync variable is posted to
		for ( int i=0; i<values[sync_variable].size(); i++ ) {
			printTime( values[sync_variable][i].first, restrict_to_backsearch );
		}
	} else {
		// print a line every sync_period seconds
		double current_time = start_time;
		while ( current_time <= stop_time ) {
			printTime( current_time, restrict_to_backsearch );
			current_time += sync_period;
		}
	}

	output.close();

	return 0;
}

string readCommandArg( string sarg ) {
	int pos = sarg.find("=");
	if ( pos==string::npos || pos==sarg.size()-1 ) {
		return "";
	} else {
		return sarg.substr( pos+1, sarg.size()-pos+1 );
	}
}

// slightly modified version of that found in lib_logutils
// always returns a string variables and allows for spaces in variable value
ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings)
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
		entry.set(atof(time.c_str()), var, src, srcaux, val);
		entry.setStatus("okay");
	} else {
		if (EOFile)
			entry.setStatus("eof");
		else
			entry.setStatus("invalid");
	}

	return (entry);
}

bool wildCardMatch( string wild, string key ) {
	if ( wild == key ) return true;
	if ( wild.empty() || key.empty() ) return false;
	if ( wild.find("*") == string::npos ) return false;

	int wild_position = wild.find("*");
	if ( wild_position == 0 ) {
		string post_wild = wild.substr(1, wild.size()-1);
		if ( key.size() < post_wild.size() ) return false;
		string key_end = key.substr( key.size()-post_wild.size(), post_wild.size() );
		if ( post_wild == key_end ) return true;
		else return false;
	} else if ( wild_position == wild.size()-1 ) {
		string pre_wild = wild.substr(0,wild_position);
		if ( key.size() < pre_wild.size() ) return false;
		string key_start = key.substr( 0, pre_wild.size() );
		if ( pre_wild == key_start ) return true;
		else return false;
	} else {
		string pre_wild = wild.substr(0,wild_position);
		string post_wild = wild.substr(wild_position+1, wild.size()-wild_position-1);
		if ( key.size() < pre_wild.size() + post_wild.size() ) return false;
		string key_begin = key.substr( 0, pre_wild.size() );
		string key_end = key.substr( key.size()-post_wild.size(), post_wild.size() );
		if ( key_begin==pre_wild && key_end==post_wild ) return true;
		else return false;
	}
}

// everything past here is identical to the moos-ivp libraries

string biteString(string& str, char separator)
{
  string::size_type len = str.length();
  if(len == 0)
    return("");

  bool found = false;
  string::size_type ix=0;
  string::size_type i=0;
  for(i=0; (!found && i<len); i++) {
    if(str[i] == separator) {
      found = true;
      ix = i;
    }
  }

  if(!found) {
    string str_front = str;
    str = "";
    return(str_front);
  }

  string str_front(str.c_str(), ix);
  string str_back;
  if((ix+1) < len)
    str_back = str.substr(ix+1);
  str = str_back;

  return(str_front);
}

string stripBlankEnds(const string& str)
{
  if(str.length() == 0)
    return("");

  const char *sPtr = str.c_str();
  int length = strlen(sPtr);

  int i;
  int startIX=length; // assume all blanks

#if 0
  // Added Dec 23rd 2007 (mikerb)
  // Quick check to see if the string has already been stripped
  // of blank ends. Just return the original in that case.
  int s_char = (int)(sPtr[0]);
  int e_char = (int)(sPtr[length-1]);
  if((s_char != 9) && (s_char != 32) &&
     (e_char != 9) && (e_char != 32))
    return(str);
#endif

  for(i=0; i<length; i++) {
    int cval = (int)(sPtr[i]);
    if(!((cval==9) ||     // 09:tab
	 (cval==32))) {   // 32:blank
      startIX = i;
      i = length;
    }
  }

  if(sPtr[startIX]==0)   // If first non-blank is NULL term,
    startIX=length;      // treat as if blank, empty line.


  if(startIX==length) {      // Handle case where the
    string ret_string = "";  // whole string was blanks
    return(ret_string);      // and/or tabs
  }

  int endIX=-1;

  for(i=length-1; i>=0; i--) {
    int cval = (int)(sPtr[i]);
    if(!((cval==9)  ||    // 09:tab
	 (cval==0)  ||    // 00:NULL terminator
	 (cval==32) ||    // 32:blank
	 (cval==13) ||    // 13:car-return
	 (cval==10))) {   // 10:newline
      endIX = i;
      i = -1;
    }
  }

  if(startIX > endIX) {      // Handle case where the
    string ret_string = "";  // whole string was blanks,
    return(ret_string);      // tabs, newlines, or car-return
  }

  length = (endIX - startIX) + 1;

  char *buff = new char [length+1];
  strncpy(buff, sPtr+startIX, length);
  buff[length] = '\0';

  string ret_string = buff;
  delete [] buff;
  return(ret_string);
}

bool isNumber(const string& str, bool blanks_allowed)
{
  string newstr = str;
  if(blanks_allowed)
    newstr = stripBlankEnds(str);

  if(newstr.length() == 0)
    return(false);

  if((newstr.length() > 1) && (newstr.at(0) == '+'))
    newstr = newstr.substr(1, newstr.length()-1);

  const char *buff = newstr.c_str();

  string::size_type  len = newstr.length();
  int  digi_cnt = 0;
  int  deci_cnt = 0;
  bool ok       = true;

  for(string::size_type i=0; (i<len)&&ok; i++) {
    if((buff[i] >= 48) && (buff[i] <= 57))
      digi_cnt++;
    else if(buff[i] == '.') {
      deci_cnt++;
      if(deci_cnt > 1)
	ok = false;
    }
    else if(buff[i] == '-') {
      if((digi_cnt > 0) || (deci_cnt > 0))
	ok = false;
    }
    else
      ok = false;
  }

  if(digi_cnt == 0)
    ok = false;

  return(ok);
}

bool scanArgs(int argc, char **argv, const char* str1,
	      const char *str2, const char *str3)
{
  for(int i=0; i<argc; i++) {
    bool match1 = !strncmp(str1, argv[i], strlen(argv[i]));
    bool match2 = !strncmp(str1, argv[i], strlen(str1));
    if(match1 && match2)
      return(true);

    if(str2) {
      match1 = !strncmp(str2, argv[i], strlen(argv[i]));
      match2 = !strncmp(str2, argv[i], strlen(str2));
      if(match1 && match2)
	return(true);
    }

    if(str3) {
      match1 = !strncmp(str3, argv[i], strlen(argv[i]));
      match2 = !strncmp(str3, argv[i], strlen(str2));
      if(match1 && match2)
	return(true);
    }
  }
  return(false);
}

bool strContains(const string& str, const string& qstr)
{
  string::size_type posn = str.find(qstr, 0);
  if(posn == string::npos)
    return(false);
  else
    return(true);
}
