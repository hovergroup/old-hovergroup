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

class ReportStruct {
public:
    ReportStruct() {
        cycle = -1;
        slot = -1;
        for (int i=0; i<4; i++){
            id.push_back(i);
            status.push_back(-1);
            x.push_back(-1);
            y.push_back(-1);
        }
    }

    void reset() {
        status = std::vector<int>(4,-1);
        x = std::vector<double>(4,-1);
        y = std::vector<double>(4,-1);
    }

    int cycle, slot;
    std::vector<int> id,status;
    std::vector<double> x,y;

    std::string serialize() {
        std::stringstream ss;
        ss << cycle << "," << slot;
        for (int i=0; i<id.size(); i++) {
            ss << ":" << id[i] << "," << status[i] <<
                    "," << x[i] << "," << y[i];
        }
        return ss.str();
    }
};

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

    ReportStruct report_struct;
};

#endif 
