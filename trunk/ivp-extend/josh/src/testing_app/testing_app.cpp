#include "RangeSensorTypes.h"
#include <iostream>

using namespace RangeSensorTypes;
using namespace std;

int main () {
	RangeRequest request;
	request.vname = "nostromo";
	request.nav_x = 19.7;
	request.nav_y = -50.2;

	cout << RangeRequest( request.toString() ).toString() << endl;

	RangeReply reply;
	reply.vname = "nostromo";
	reply.range = 100.2;
	cout << RangeReply( reply.toString() ).toString() << endl;
}
