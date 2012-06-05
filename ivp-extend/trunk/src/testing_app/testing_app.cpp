/*
 * testing_app.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

#include <iostream>
//#include <acomms_messages.h>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;
//using namespace lib_acomms_messages;

int main() {
	string date_string = "191112";
	string time_string = "225446.123";
	string modified_date = "20"+date_string.substr(4,2)+
			date_string.substr(2,2)+
			date_string.substr(0,2);
	string composite = modified_date+"T"+time_string;

	cout << composite << endl;

	ptime t(from_iso_string(composite));

	cout << to_simple_string(t) << endl;

	time_duration td = t.time_of_day();

	cout << to_simple_string( td ) << endl;

	double seconds = td.total_milliseconds()/1000.0;

	cout << seconds << endl;

//	cout << to_simple_string(time_from_string(to_simple_string(t))) << endl;
//	SIMPLIFIED_RECEIVE_INFO info;
//	info.vehicle_name = "bob";
//	info.num_frames = 3;
//	info.num_good_frames = 2;
//	info.num_bad_frames = 1;
//	info.rate = 2;
//
//	cout << info.serializeToString() << endl;
//
//	SIMPLIFIED_RECEIVE_INFO info2( info.serializeToString() );
//
//	cout << info2.serializeToString() << endl;

	return 0;
}


