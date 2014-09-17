/*
 * pPursuitShore
 *        File: PursuitShore.h
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#ifndef PursuitShore_HEADER
#define PursuitShore_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "JoshUtils.h"
#include "pursuit.pb.h"
#include "goby/acomms/dccl.h"
#include "HoverAcomms.h"
#include "XYFormatUtilsSegl.h"

class PursuitShore: public CMOOSApp {
public:
    PursuitShore();
    ~PursuitShore();

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

    PursuitCommandDCCL dccl_command;
    goby::acomms::DCCLCodec *encoder, *decoder;

    std::vector<int> receive_counts;
};

#endif 
