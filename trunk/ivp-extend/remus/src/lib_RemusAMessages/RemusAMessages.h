/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RemusAMessages.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RemusAMessages_HEADER
#define RemusAMessages_HEADER

#include <string>

namespace RemusAMessages {
class RemusStatusM {
public:
    RemusStatusM() {};
    RemusStatusM( std::string msg );

    std::string vname;
    double nav_x, nav_y, nav_d, nav_b, nav_s; //[x y depth,bearing,speed]

    std::string toString();
};

class RemusCmdM {
public:
    RemusCmdM() {};
    RemusCmdM( std::string msg );

    std::string cmd;

    std::string toString();
};


class Remus13Bits {
public:
    Remus13Bits(){};

    unsigned char  LinearEncode( double val, double min, double max, int bits );
    double  LinearDecode( unsigned char val, double min, double max, int bits );

    // very bad way of encoding, just for temporary usage
    std::string Num2Cmd(int numCmd);
    int Cmd2Num(std::string cmd);
};


}

#endif 
