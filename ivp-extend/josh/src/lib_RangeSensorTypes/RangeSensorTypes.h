/*
 * RangeSensorTypes.h
 *
 *  Created on: Oct 16, 2012
 *      Author: josh
 */

#ifndef RANGESENSORTYPES_H_
#define RANGESENSORTYPES_H_

#include <string>

namespace RangeSensorTypes {
	class RangeRequest {
	public:
		RangeRequest() {};
		RangeRequest( std::string msg );

		std::string vname;
		double nav_x, nav_y;

		std::string toString();
	};

	class RangeReply {
	public:
		RangeReply() {};
		RangeReply( std::string msg );

		std::string vname;
		double range;

		std::string toString();
	};
}


#endif /* RANGESENSORTYPES_H_ */
