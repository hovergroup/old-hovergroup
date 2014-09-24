/*
 * pTDOAShore
 *        File: TDOAShore.h
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#ifndef TDOAShore_HEADER
#define TDOAShore_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "JoshUtils.h"
#include "tdoa.pb.h"
#include "goby/acomms/dccl.h"
#include "HoverAcomms.h"
#include "XYFormatUtilsSegl.h"

class TDOAShore: public CMOOSApp {
public:
    TDOAShore();
    ~TDOAShore();

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

    TDOACommandDCCL dccl_command;
    goby::acomms::DCCLCodec* codec;

    std::vector<int> receive_counts;
    std::vector<bool> sent_reports, got_commands;
    bool got_receive;
};

#endif 
