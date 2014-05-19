/*
 * pNavManager
 *        File: NavManager.h
 *  Created on: Oct 18, 2013
 *      Author: Josh Leighton
 */

#ifndef NavManager_HEADER
#define NavManager_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "XYPoint.h"
#include <vector>

enum NAV_SOURCE {
    gps = 0, rtk, none
};

class NavManager: public CMOOSApp {
public:
    NavManager();
    ~NavManager();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();

private:
    double TIMEOUT;

private:
    NAV_SOURCE source;
    bool gps_lock, rtk_available, gps_available;
    double gps_update_time, rtk_update_time;

    double alt_x, alt_y;
    double last_point_post_time;

    double last_source_post_time;

    std::string my_name;

    void setSource(NAV_SOURCE new_val);
    void postSource();

    enum RTK_STATUS {
        FIX, FLOAT, SINGLE, NONE
    };
    RTK_STATUS rtk_status;

    enum DETAILED_SOURCE {
        gps_internal, rtk_fix, rtk_float, rtk_single
    };
    std::vector<DETAILED_SOURCE> source_priorities;
};

#endif
