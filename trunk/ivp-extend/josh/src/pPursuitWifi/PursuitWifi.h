/*
 * pPursuitWifi
 *        File: PursuitWifi.h
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#ifndef PursuitWifi_HEADER
#define PursuitWifi_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "JoshUtils.h"
#include "pursuit.pb.h"
#include "goby/acomms/dccl.h"
#include "HoverAcomms.h"
#include "XYFormatUtilsSegl.h"

class PursuitWifi: public CMOOSApp {
public:
    PursuitWifi();
    ~PursuitWifi();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

private:
    // Configuration variables

private:
    JoshUtil::TDMAEngine tdma_engine;

    std::vector<double> m_navx, m_navy;
    std::vector<double> m_waypointx, m_waypointy;
    bool m_running;
};

#endif 
