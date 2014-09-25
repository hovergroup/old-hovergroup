/*
 * pTDOAShore
 *        File: TDOAShore.cpp
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#include <iterator>
#include "MBUtils.h"
#include "TDOAShore.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TDOAShore::TDOAShore() {
    receive_counts = vector<int>(3,0);
    sent_reports = vector<bool>(3,false);
    got_commands = vector<bool>(3,false);

    codec = goby::acomms::DCCLCodec::get();
    try {
        codec->validate<TDOAReportDCCL>();
        codec->validate<TDOACommandDCCL>();
    } catch (goby::acomms::DCCLException& e) {
        std::cout << "failed to validate encoder" << std::endl;
    }
    got_receive = false;
}

//---------------------------------------------------------
// Destructor

TDOAShore::~TDOAShore() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TDOAShore::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == "TDOA_COMMAND") {
            string cmd = MOOSToUpper(msg.GetString());
            if (cmd == "RUN") {
                tdma_engine.run();
            } else if (cmd == "STOP") {
                tdma_engine.stop();
            } else if (cmd == "RESET") {
                tdma_engine.reset();
                receive_counts = vector<int>(3,0);
            }
        } else if (key == "TDOA_VEHICLE_COMMAND") {
            string line = msg.GetString();
            int id = atoi(MOOSChomp(line, ",").c_str());
            double x = atof(MOOSChomp(line, ",").c_str());
            double y = atof(line.c_str());
            cout << "got command for id: " << id << endl;
            TDOATrio *trio = dccl_command.add_data();
            trio->set_id(id);
            trio->set_x(x);
            trio->set_y(y);
            got_commands[id-1] = true;
        } else if (key == "ACOMMS_RECEIVED" && MOOSTime()-msg.GetTime() < 1) {
            HoverAcomms::AcommsReception reception;
            // try to parse acomms message
            if (reception.parseFromString(msg.GetString())) {
                // check status
                if (reception.getStatus() == HoverAcomms::GOOD) {

                    // perform decode
                    TDOAReportDCCL report;
                    bool decode_okay = true;
                    try {
                        cout << "attempting to decode " << reception.getData().size() << " bytes" << endl;
//                        cout << reception.getData() << endl;
//                        for (int i=0; i<reception.getData().size(); i++) {
//                            cout << (int) reception.getData()[i] << " ";
//                        }
//                        cout << endl;
//                        for (int i=0; i<reception.getData().size(); i++) {
//                            std::bitset<8> x(reception.getData()[i]);
//                            cout << x << " ";
//                        }
//                        cout << endl;
                        codec->decode(reception.getData(), &report);
                    } catch (goby::acomms::DCCLException &) {
                        stringstream ss;
                        ss << "shoreside failed decoding acomms message" << endl;
                        cout << ss.str() << endl;
                        m_Comms.Notify("TDOA_ERROR", ss.str());
                        decode_okay = false;
                    }

                    if (decode_okay) {
                        receive_counts[reception.getSource()-1]++;
                        stringstream ss;
                        for (int i=0; i<receive_counts.size(); i++) {
                            if (i!=0)
                                ss << ",";
                            ss << receive_counts[i];
                        }
                        m_Comms.Notify("TDOA_RECEIVE_COUNTS", ss.str());
                        cout << "received data: " << endl;
                        cout << report.DebugString() << endl;

                        report_struct.slot = reception.getSource();
                        for (int i=0; i<report.data_size(); i++) {
                            int id = report.data(i).id();
                            if (id != 0) {
                                if (!sent_reports[id-1]) {
                                    report_struct.status[id] = 1;
                                    report_struct.x[id] = report.data(i).x();
                                    report_struct.y[id] = report.data(i).y();
                                    sent_reports[id-1] = true;
                                }
                            }
                        }

                        m_Comms.Notify("TDOA_VEHICLE_REPORT", report_struct.serialize());;
                        report_struct.reset();

                        got_receive = true;
                    }
                }
            }
        }

    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TDOAShore::OnConnectToServer() {

    tdma_engine.parseConfig(m_MissionReader, GetAppName());
    cout << tdma_engine.getSummary() << endl;
    m_Comms.Notify("TDMA_SUMMARY", tdma_engine.getSingleLineSummary());

    m_Comms.Register("TDOA_COMMAND", 0);
    m_Comms.Register("ACOMMS_RECEIVED", 0);
    m_Comms.Register("TDOA_VEHICLE_COMMAND");

    return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOAShore::Iterate() {
    if (tdma_engine.testAdvance()) {
        m_Comms.Notify("TDMA_SLOT", tdma_engine.getCurrentSlot());
        m_Comms.Notify("TDMA_CYCLE_COUNT", tdma_engine.getCycleCount());
        m_Comms.Notify("TDMA_CYCLE_NUMBER", tdma_engine.getCycleNumber());

        int slot = tdma_engine.getCurrentSlot();

        if (slot == 4) {
            got_commands = vector<bool>(3,false);
            dccl_command.Clear();
            cout << "clearing command buffer" << endl;
        }

        if (slot == 0) {
            report_struct.reset();
            report_struct.cycle = tdma_engine.getCycleNumber();
            sent_reports = vector<bool>(3,false);
            cout << "clearing report record" << endl;
        }

        // range ping timing
        if (slot == 1) {
            report_struct.slot = 0;
            report_struct.status[0] = 1;
            m_Comms.Notify("TDOA_VEHICLE_REPORT", report_struct.serialize());
            report_struct.status[0] = -1;
        }

        // if our slot, send update
        if (tdma_engine.getCurrentSlot() == 6) {
            bool got_command = true;
            for (int i=0; i<got_commands.size(); i++) {
                if (got_commands[i] == false) {
                    got_command = false;
                    break;
                }
            }
//            if (got_command) {
                std::string bytes;
                codec->encode(&bytes, dccl_command);
                cout << "Transmitting: " << endl;
                cout << dccl_command.DebugString() << endl;
                dccl_command.Clear();
                m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(), bytes.size());
//            }
            if (!got_command) {
                stringstream ss;
                ss << "shoreside has no command to send" << endl;
                cout << ss.str() << endl;
                m_Comms.Notify("TDOA_ERROR", ss.str());
            }
        }

        if (!got_receive && (slot==3 || slot==4 || slot==5)) {
            cout << "Missed receive before slot " << slot << endl;
            int id;
            switch (slot) {
            case 3:
                id = 1;
                break;
            case 4:
                id = 2;
                break;
            case 5:
                id=3;
                break;
            }
            report_struct.reset();
            report_struct.slot = id;
            report_struct.status[id] = 0;
            m_Comms.Notify("TDOA_VEHICLE_REPORT", report_struct.serialize());
            report_struct.reset();
        }

        got_receive = false;
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TDOAShore::OnStartUp() {
    return (true);
}

