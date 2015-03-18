/*
 * mc2922
 *        File: PointAssign.h
 *  Created on: Mar 18, 2015
 *      Author: mc2922
 */

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <vector>
#include <boost/lexical_cast.hpp>

class PointAssign : public CMOOSApp
{
public:
    PointAssign();
    ~PointAssign();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

private:

    std::vector<double> xvec;
    std::vector<double> yvec;
    std::vector<double> idvec;
    bool assign_by_region;
};

#endif 
