/*
 * lib_receive_info.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

#include "lib_receive_info.h";

using namespace lib_receive_info;
using namespace std;

SIMPLIFIED_RECEIVE_INFO::SIMPLIFIED_RECEIVE_INFO( string msg ) {
	vector<string> substrings;
	int pos = 0;
	while ( msg.find(":", pos) != string::npos ) {
		int newpos = msg.find(":");
		substrings.push_back( msg.substr(pos, newpos-1) );
		pos = newpos+1;
	}
	substrings.push_back( msg.substr(pos, msg.size()-pos) );


}

string SIMPLIFIED_RECEIVE_INFO::serializeToString() {
	stringstream ss;

	ss << "vehicle_name," << vehicle_name;
	ss << ":rate," << rate;
	ss << ":num_frames," << num_frames;
	ss << ":num_good_frames," << num_good_frames;
	ss << ":num_bad_frames," << num_bad_frames;

	return ss.str();
}
