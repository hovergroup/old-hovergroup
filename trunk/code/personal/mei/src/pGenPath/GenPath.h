/*
 * mc2922
 *        File: GenPath.h
 *  Created on: Mar 18, 2015
 *      Author: mc2922
 */

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class GenPath : public CMOOSApp
{
public:
    GenPath();
    ~GenPath();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

private:
};

#endif 
