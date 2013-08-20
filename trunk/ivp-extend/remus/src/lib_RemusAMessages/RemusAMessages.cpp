/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RemusAMessages.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include "RemusAMessages.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include <math.h>

using namespace RemusAMessages;

//---------------------------------------------------------
// =================== RemusStatusM =======================

RemusStatusM::RemusStatusM(std::string msg)
{
    MOOSChomp(msg, "=");
    vname = MOOSChomp(msg, ",");
    MOOSChomp(msg, "=");
    nav_x = atof( MOOSChomp(msg, ",").c_str() );
    MOOSChomp(msg, "=");
    nav_y = atof( MOOSChomp(msg, ",").c_str() );
    MOOSChomp(msg, "=");
    nav_d = atof( MOOSChomp(msg, ",").c_str() );
    MOOSChomp(msg, "=");
    nav_b = atof( MOOSChomp(msg, ",").c_str() );
    MOOSChomp(msg, "=");
    nav_s = atof( msg.c_str() );
}

std::string RemusStatusM::toString()
{
    std::stringstream ss;
    ss << "vname=" << vname
            << ",x=" << nav_x
            << ",y=" << nav_y
            << ",d=" << nav_d
            << ",b=" << nav_b
            << ",s=" << nav_s;
    return ss.str();
}

// =========================================

RemusCmdM::RemusCmdM( std::string msg ) {
    //	MOOSChomp(msg, "=");
    cmd = msg.c_str();
}

std::string RemusCmdM::toString() {
    std::stringstream ss;
    ss  << cmd;
    return ss.str();
}

unsigned char Remus13Bits::LinearEncode(double val, double min, double max,
        int bits) {
    unsigned char transmit_val;
    if (val <= min)
        return 0;
    else if (val >= max)
        return pow(2, bits) - 1;
    else {
        double ratio = (val - min) / (max - min);
        double scaled = ratio * (pow(2, bits) - 1);
        return floor(scaled + .5);
    }
}

double Remus13Bits::LinearDecode(unsigned char val, double min, double max,
        int bits) {
    double ratio = val / (pow(2.0, bits) - 1.0);
    return min + ratio * (max - min);
}

std::string Remus13Bits::Num2Cmd(int numCmd){
    switch(numCmd){
    case 1: return "INACTIVE";
    case 2: return "GOTO";
    case 3: return "STATION-KEEP";
    case 4: return "RETURN";
    case 5: return "STARTLOITER";
    case 6: return "STARTMISSION";
    default: return "INACTIVE";
    }
}


unsigned char Remus13Bits::Cmd2Num(std::string cmd){
    if (cmd.compare("INACTIVE")==0)
        return 1;
    else if (cmd.compare("GOTO")==0)
        return 2;
    else if (cmd.compare("STATION-KEEP")==0)
        return 3;
    else if (cmd.compare("RETURN")==0)
        return 4;
    else if (cmd.compare("STARTLOITER")==0)
        return 5;
    else if (cmd.compare("STARTMISSION")==0)
        return 6;
    else
        return 0;
}



