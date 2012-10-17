#include "RangeSensorTypes.h"
#include <iostream>
#include <math.h>

using namespace RangeSensorTypes;
using namespace std;


unsigned char LinearEncode( double val, double min, double max, int bits ) {
	unsigned char transmit_val;
	if ( val <= min ) return 0;
	else if ( val >= max ) return pow(2,bits)-1;
	else {
		double ratio = (val-min)/(max-min);
		double scaled = ratio * (pow(2,bits)-1);
		return floor( scaled + .5 );
	}
}

double LinearDecode( unsigned char val, double min, double max, int bits ) {
	double ratio = val / ( pow(2.0,bits) - 1.0 );
	return min + ratio * ( max - min );
}


int main () {
	double val = 310;
	unsigned char sent = LinearEncode( val, 0, 310, 5 );
	cout << (int) sent << endl;
	double received = LinearDecode( sent, 0, 310, 5);
	cout << received << endl;
//	cout << (int) LinearEncode( val, 0, 310, 5) << endl;
//	cout << hex << (int) LinearEncode( val, 0, 310, 5) << endl;
//	cout << hex << (int) (LinearEncode( val, 0, 310, 5)<<3) << endl;
}
