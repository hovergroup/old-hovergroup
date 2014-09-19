/*
 * pPursuitVehicle
 *        File: PursuitVehicle.h
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#ifndef PursuitVehicle_HEADER
#define PursuitVehicle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "JoshUtils.h"
#include "pursuit.pb.h"
#include "goby/acomms/dccl.h"
#include "HoverAcomms.h"
#include "XYFormatUtilsSegl.h"

class PursuitVehicle: public CMOOSApp {
public:
    PursuitVehicle();
    ~PursuitVehicle();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

private:
    // Configuration variables

private:
    JoshUtil::TDMAEngine tdma_engine;
    bool m_running;

    double m_navx, m_navy;
    int m_id, m_shore_id;

    PursuitReportDCCL dccl_report;
    goby::acomms::DCCLCodec* codec;
    std::vector<int> command_trajectory;
    std::map<int,double> command_map;

    double positive_x, positive_y, negative_x, negative_y;

    int receive_count;
};

#endif 
