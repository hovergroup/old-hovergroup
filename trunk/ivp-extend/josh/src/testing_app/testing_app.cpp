#include "RangeSensorTypes.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>

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

unsigned char FlexibleEncode(double val,
        std::vector<double> & range_divs, int bits) {

    if (range_divs.size() != pow(2, bits) - 1) {
        std::stringstream ss;
        ss << "Specified number of bits (" << bits;
        ss << ") does not match vector size (" << range_divs.size();
        ss << ")";
        cout << ss.str() << endl;
//        handleDebug(ss.str());
        return 0x00;
    }

    unsigned char transmit_val;

    for (int i = 0; i < range_divs.size() - 1; i++) {
        if ( val < range_divs[i] )
            return i;
    }
    return range_divs.size()-1;
}



int main () {
    unsigned char a = 22;
    unsigned char b = 4;
    cout << hex << (int) ( (a<<3) + b ) << endl;

//    std::vector<double> range_divs;
//    for ( int i=2; i<9; i++ ) {
//        range_divs.push_back(i*10);
//    }
//
//    unsigned char range =FlexibleEncode(55,range_divs,3);
//    range+=0xf8;
//
//    cout << (int) (range & 0x07)<< endl;

//	double val = 310;
//	unsigned char sent = LinearEncode( val, 0, 310, 5 );
//	cout << (int) sent << endl;
//	double received = LinearDecode( sent, 0, 310, 5);
//	cout << received << endl;
//	cout << (int) LinearEncode( val, 0, 310, 5) << endl;
//	cout << hex << (int) LinearEncode( val, 0, 310, 5) << endl;
//	cout << hex << (int) (LinearEncode( val, 0, 310, 5)<<3) << endl;
}
