/*
 * JoshUtil.h
 *
 *  Created on: Nov 11, 2012
 *      Author: josh
 */

#ifndef LIB_HOVERGEOMETRY_H_
#define LIB_HOVERGEOMETRY_H_

#include "geometry.pb.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include <string>

namespace HoverGeometry {

bool parseViewPoint(std::string view_point, VIEW_POINT & proto);

};

#endif // LIB_HOVERGEOMETRY_H_
