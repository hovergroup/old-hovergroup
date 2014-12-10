/*
 * pTDOAVehicle
 *        File: TDOAVehicle.h
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#ifndef TDOAVehicle_HEADER
#define TDOAVehicle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "JoshUtils.h"
#include "tdoa.pb.h"
#include "goby/acomms/dccl.h"
#include "HoverAcomms.h"
#include "XYFormatUtilsSegl.h"

class TDOAVehicle: public CMOOSApp {
public:
    TDOAVehicle();
    ~TDOAVehicle();

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

    TDOAReportDCCL dccl_report;
    goby::acomms::DCCLCodec* codec;
    std::vector<int> receive_counts;

    bool got_command;
};

#endif 
