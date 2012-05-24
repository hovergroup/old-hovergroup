/*
 * lib_receive_info.h
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

#ifndef LIB_RECEIVE_INFO_H_
#define LIB_RECEIVE_INFO_H_

#include <sstream>

namespace lib_receive_info {

class SIMPLIFIED_RECEIVE_INFO {
public:
	std::string vehicle_name;
	int num_frames, num_good_frames, num_bad_frames;
	int rate;

	std::string serializeToString();

};

}

#endif /* LIB_RECEIVE_INFO_H_ */
