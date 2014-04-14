/*
 * JoshUtil.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: josh
 */

#include "HoverGeometry.h"

using namespace HoverGeometry;
using namespace std;

bool HoverGeometry::parseViewPoint(string view_point, VIEW_POINT & proto) {
	double x, y, z;
	bool ok1 = MOOSValFromString(x, view_point, "x");
	bool ok2 = MOOSValFromString(y, view_point, "y");
	proto.set_x(x);
	proto.set_y(y);
	if (MOOSValFromString(z, view_point, "z")) {
		proto.set_z(z);
	}

	string label, msg;
	bool ok3 = MOOSValFromString(label, view_point, "label");
	proto.set_label(label);
	if (MOOSValFromString(msg, view_point, "msg")) {
		proto.set_msg(msg);
	}
	return ok1 && ok2 && ok3;
}

