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

    string view_seglist = "pts={80,-120.4:30,0},label=NOSTROMO_tulip_goto,vertex_color=red,vertex_size=2,edge_size=3,active=false";
//    string view_seglist="lawnmower: x=0, y=8, width=100, height=80, degs=45, swath=20, startx=-40, starty=80, start=tl, label=lawn";
    VIEW_SEGLIST vs;
    cout << parseSeglist(view_seglist, vs) << endl;
    cout << vs.DebugString() << endl;
    cout << "size = " << view_seglist.size() << "  " << vs.SerializeAsString().size() << endl;
    cout << printSeglist(vs) << endl << endl;

    string view_marker = "x=50,y=-50,width=5,type=diamond,color=red,label=mark,active=false";
    VIEW_MARKER mark;
    cout << parseMarker(view_marker, mark) << endl;
    cout << mark.DebugString() << endl;
    cout << "size = " << view_marker.size() << "  " << mark.SerializeAsString().size() << endl;
    cout << printMarker(mark) << endl << endl;

	string view_point = "x=40,y=-120,active=false,label=NOSTROMO's next waypoint,label_color=green,vertex_color=red,vertex_size=4";
	VIEW_POINT vp;
	cout << parsePoint(view_point, vp) << endl;
	cout << vp.DebugString() << endl;
	cout << "size = " << view_point.size() << "  " << vp.SerializeAsString().size() << endl;
	cout << printPoint(vp) << endl;

	string view_polygon = "radial:: x=48.3,y=-120.2,source=NOSTROMO:tulip_stationkeep,pts=32,label=NOSTROMO:station-keep-out,radius=5.0,active=false";
	return 0;
}
