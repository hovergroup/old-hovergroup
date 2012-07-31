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
std::string fixToString( ALogEntry entry, int line_num );

// functions copied from moos-ivp to support separate compilation
std::string biteString(std::string& str, char separator);
std::string stripBlankEnds(const std::string& str);
bool isNumber(const std::string& str, bool blanks_allowed=true);
bool scanArgs(int, char**, const char*, const char *a=0, const char *b=0);
bool strContains(const std::string& str, const std::string& qstr);

using namespace std;

map<string,vector<pair<double,string> > > values;
vector<string> variables;

string sync_variable;
bool use_sync_variable = false;
double sync_period = 0;

void printHelp() {

}

// find the nearest entry by time, returns index and time diff
template <class T>
pair<int,double> findNearest( vector< pair<double,T> > item_list, double msg_time ) {
	if ( item_list.empty() ) return pair<int,double>(-1,-1);
	double min_diff = 10000;
	int index = -1;
	for ( int i=0; i<item_list.size(); i++ ) {
		if ( fabs(item_list[i].first-msg_time) < min_diff ) {
			min_diff = fabs(item_list[i].first-msg_time);
			index = i;
		}
	}
	double age = msg_time - item_list[index].first;
	return pair<int,double>(index,age);
}

ofstream output;

void printHeader() {
	output << "time";
	for ( int i=0; i<variables.size(); i++ ) {
		output << "," << variables[i] << "," << "age";
	}
	output << endl;
}

void printTime( double msg_time ) {
	output << msg_time;
	for ( int i=0; i<variables.size(); i++ ) {
		string var = variables[i];
		pair<int,double> result = findNearest<string>(values[var], msg_time);
		if ( result.first == -1 )
			output << ",-1,-1";
		else
			output << "," << values[var][result.first].second << "," << result.second;
	}
	output << endl;
}

int main (	int argc, char *argv[] ) {
	if( scanArgs(argc, argv, "-h", "--help" ) ) {
		printHelp();
		return 0;
	}

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
		} else {
			variables.push_back( string(sarg) );
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


//	if ( use_sync_variable )
//		cout << "syncing on " << sync_variable << endl;
//	else
//		cout << "syncing every " << sync_period << endl;
//	cout << "looking for: ";
//	for ( int i=0; i<variables.size(); i++ ) {
//		cout << variables[i] << " ";
//	}
//	cout << endl;


	double start_time = -100;
	double stop_time = -100;
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
				if ( start_time == -100 )
					start_time = msg_time;
				if ( msg_time > stop_time )
					stop_time = msg_time;
			}
		}
		entry = getNextRawALogEntry_josh( logfile );
	}

//	cout << "read done" << endl;

	string file_out = alogfile_in;
	file_out.replace( file_out.size()-4, 4, "txt");
	output.open(file_out.c_str());
	printHeader();

	if ( use_sync_variable ) {
		for ( int i=0; i<values[sync_variable].size(); i++ ) {
			printTime( values[sync_variable][i].first );
		}
	} else {
		double current_time = start_time;
		while ( current_time <= stop_time ) {
			printTime( current_time );
			current_time += sync_period;
		}
	}

	output.close();

	return 0;
}

// force an alog entry to be a string
string fixToString( ALogEntry entry, int line_num ) {
	if ( entry.isNumerical() ) {
		cout << "WARNING at line " << line_num <<
				": " << entry.getVarName() << " was numerical, will convert." << endl;
		stringstream ss;
		ss << entry.getDoubleVal();
		return ss.str();
	} else {
		return entry.getStringVal();
	}
}

string readCommandArg( string sarg ) {
	int pos = sarg.find("=");
	if ( pos==string::npos || pos==sarg.size()-1 ) {
		return "";
	} else {
		return sarg.substr( pos+1, sarg.size()-pos+1 );
	}
}

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
