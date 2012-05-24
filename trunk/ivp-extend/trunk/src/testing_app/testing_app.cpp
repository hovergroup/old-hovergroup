/*
 * testing_app.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

#include <iostream>
#include <lib_receive_info.h>

using namespace std;
using namespace lib_receive_info;

int main() {
	SIMPLIFIED_RECEIVE_INFO info;
	info.vehicle_name = "bob";
	info.num_frames = 3;
	info.num_good_frames = 2;
	info.num_bad_frames = 1;
	info.rate = 2;

	cout << info.serializeToString() << endl;



	return 0;
}


