/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RemusAMessages.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include "RemusAMessages.h"
#include "MOOS/libMOOS/MOOSLib.h"

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
	nav_b = atof( msg.c_str() );
}

std::string RemusStatusM::toString()
{
	std::stringstream ss;
	ss << "vname=" << vname
			<< ",x=" << nav_x
			<< ",y=" << nav_y
			<< ",y=" << nav_d
			<< ",y=" << nav_b;
	return ss.str();
}

// =========================================

RemusCmdM::RemusCmdM( std::string msg ) {
	MOOSChomp(msg, "=");
	cmd = msg.c_str();
}

std::string RemusCmdM::toString() {
	std::stringstream ss;
	ss << ",cmd=" << cmd;
	return ss.str();
}


