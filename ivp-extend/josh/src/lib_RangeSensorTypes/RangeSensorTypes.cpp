/*
 * RangeSensorTypes.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: josh
 */

#include "RangeSensorTypes.h"
#include "MOOS/libMOOS/MOOSLib.h"

using namespace RangeSensorTypes;

RangeRequest::RangeRequest( std::string msg ) {
	MOOSChomp(msg, "=");
	vname = MOOSChomp(msg, ",");
	MOOSChomp(msg, "=");
	nav_x = atof( MOOSChomp(msg, ",").c_str() );
	MOOSChomp(msg, "=");
	nav_y = atof( msg.c_str() );
}

std::string RangeRequest::toString() {
	std::stringstream ss;
	ss << "vname=" << vname
	   << ",x=" << nav_x
	   << ",y=" << nav_y;
	return ss.str();
}

RangeReply::RangeReply( std::string msg ) {
	MOOSChomp(msg, "=");
	vname = MOOSChomp(msg, ",");
	MOOSChomp(msg, "=");
	range = atof( msg.c_str() );
}

std::string RangeReply::toString() {
	std::stringstream ss;
	ss << "vname=" << vname
	   << ",range=" << range;
	return ss.str();
}
