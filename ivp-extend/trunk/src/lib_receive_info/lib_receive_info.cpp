/*
 * lib_receive_info.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */
#include "lib_receive_info.h";

using namespace lib_receive_info;
using namespace std;

string SIMPLIFIED_RECEIVE_INFO::serializeToString() {
	stringstream ss;

	ss << "vehicle_name," << vehicle_name;
	ss << ":num_frames," << num_frames;

	return ss.str();
}
