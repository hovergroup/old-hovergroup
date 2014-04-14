#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "HoverGeometry.h"
#include "geometry.pb.h"

using namespace std;
using namespace HoverGeometry;

int main(int argc,char **argv)
{
	string view_point = "x=5,y=10,label=test";
	VIEW_POINT vp;
	cout << parseViewPoint(view_point, vp) << endl;
	cout << vp.DebugString() << endl;
	return 0;
}
