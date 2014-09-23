/*
 * pPursuitShore
 *        File: PursuitShore.cpp
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#include <iterator>
#include "MBUtils.h"
#include "PursuitShore.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PursuitShore::PursuitShore() {
    receive_counts = vector<int>(3,0);
    got_commands = vector<bool>(3,false);

    codec = goby::acomms::DCCLCodec::get();
    try {
        codec->validate<PursuitReportDCCL>();
        codec->validate<PursuitCommandDCCL>();
    } catch (goby::acomms::DCCLException& e) {
        std::cout << "failed to validate encoder" << std::endl;
    }
    got_receive = false;
    multicast = true;
}

//---------------------------------------------------------
// Destructor

PursuitShore::~PursuitShore() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PursuitShore::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == "PURSUIT_COMMAND") {
            string cmd = MOOSToUpper(msg.GetString());
            if (cmd == "RUN") {
                tdma_engine.run();
            } else if (cmd == "STOP") {
                tdma_engine.stop();
            } else if (cmd == "RESET") {
                tdma_engine.reset();
                receive_counts = vector<int>(3,0);
            }
        } else if (key == "PURSUIT_VEHICLE_COMMAND") {
            string line = msg.GetString();
            int id = atoi(MOOSChomp(line, ",").c_str());
            vector<int> trajectory;
            while (line.find(",") != string::npos) {
                trajectory.push_back(atoi(MOOSChomp(line,",").c_str()));
            }
            trajectory.push_back(atoi(line.c_str()));
            cout << "got trajectory for id: " << id << endl;
            switch (id) {
            case 1:
                dccl_command.clear_vehicle1_cmd();
                for (int i=0; i<trajectory.size(); i++) {
                    dccl_command.add_vehicle1_cmd(trajectory[i]);
                }
                break;
            case 2:
                dccl_command.clear_vehicle2_cmd();
                for (int i=0; i<trajectory.size(); i++) {
                    dccl_command.add_vehicle2_cmd(trajectory[i]);
                }
                break;
            case 3:
                dccl_command.clear_vehicle3_cmd();
                for (int i=0; i<trajectory.size(); i++) {
                    dccl_command.add_vehicle3_cmd(trajectory[i]);
                }
                break;
            }
            got_commands[id-1] = true;
        } else if (key == "ACOMMS_RECEIVED" && MOOSTime()-msg.GetTime() < 1) {
            HoverAcomms::AcommsReception reception;
            // try to parse acomms message
            if (reception.parseFromString(msg.GetString())) {
                // check status
                if (reception.getStatus() == HoverAcomms::GOOD) {

                    // perform decode
                    PursuitReportDCCL report;
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
                        m_Comms.Notify("PURSUIT_ERROR", ss.str());
                        decode_okay = false;
                    }

                    if (decode_okay) {
                        receive_counts[report.id()-1]++;
                        stringstream ss;
                        for (int i=0; i<receive_counts.size(); i++) {
                            if (i!=0)
                                ss << ",";
                            ss << receive_counts[i];
                        }
                        m_Comms.Notify("PURSUIT_RECEIVE_COUNTS", ss.str());
                        cout << "received data: " << endl;
                        cout << report.DebugString() << endl;

                        ss.str("");
                        ss << report.id() << ":1:";
                        if (report.ack()) {
                            ss << "1:";
                        } else {
                            ss << "0:";
                        }
                        ss << tdma_engine.getCycleCount();
                        for (int i=0; i<report.slot_history_size(); i++) {
                            ss << ":" << report.slot_history(i);
                            ss << "," << report.x_history(i);
                            ss << "," << report.y_history(i);
                        }
                        m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss.str());

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

bool PursuitShore::OnConnectToServer() {

    tdma_engine.parseConfig(m_MissionReader, GetAppName());
    cout << tdma_engine.getSummary() << endl;
    m_Comms.Notify("TDMA_SUMMARY", tdma_engine.getSingleLineSummary());

    m_Comms.Register("PURSUIT_COMMAND", 0);
    m_Comms.Register("ACOMMS_RECEIVED", 0);
    m_Comms.Register("PURSUIT_VEHICLE_COMMAND");

    string mode;
    m_MissionReader.GetConfigurationParam("command_mode", mode);
    MOOSToUpper(mode);
    if (mode == "MULTICAST") {
        multicast = true;
    } else if (mode == "INTERLEAVED") {
        multicast = false;
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PursuitShore::Iterate() {
    if (tdma_engine.testAdvance()) {
        m_Comms.Notify("TDMA_SLOT", tdma_engine.getCurrentSlot());
        m_Comms.Notify("TDMA_CYCLE_COUNT", tdma_engine.getCycleCount());
        m_Comms.Notify("TDMA_CYCLE_NUMBER", tdma_engine.getCycleNumber());

        if (multicast) {
            if (tdma_engine.getCurrentSlot() == 5) {
                got_commands = vector<bool>(3,false);
                cout << "clearing command buffer" << endl;
            }

            if (tdma_engine.getCurrentSlot() == 1) {
                stringstream ss1;
                ss1 << "-1:-1:-1:";
                ss1 << tdma_engine.getCycleCount();
                m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss1.str());
            }

            // if our slot, send update
            if (tdma_engine.getCurrentSlot() == 0) {
                bool got_command = true;
                for (int i=0; i<got_commands.size(); i++) {
                    if (got_commands[i] == false) {
                        got_command = false;
                        break;
                    }
                }
                if (got_command) {
                    std::string bytes;
                    codec->encode(&bytes, dccl_command);
                    cout << "Transmitting: " << endl;
                    cout << dccl_command.DebugString() << endl;
                    dccl_command.Clear();
                    m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(), bytes.size());
                } else {
                    stringstream ss;
                    ss << "shoreside has no command to send" << endl;
                    cout << ss.str() << endl;
                    m_Comms.Notify("PURSUIT_ERROR", ss.str());
                }
            }

            int slot = tdma_engine.getCurrentSlot();
            if (!got_receive && (slot==3 || slot==5 || slot==7)) {
                cout << "Missed receive before slot " << slot << endl;
                stringstream ss;
                ss << slot-1 << ":0:0:";
                ss << tdma_engine.getCycleCount();
                m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss.str());
            } else {
                got_receive = false;
            }
        } else {
            int slot = tdma_engine.getCurrentSlot();

            if (slot==1 || slot==4 || slot==5) {
                got_commands = vector<bool>(3,false);
                cout << "clearing command buffer" << endl;
            }

            // if command slot, send update
            if (slot == 0) {
                if (got_commands[0]) {
                    std::string bytes;
                    codec->encode(&bytes, dccl_command);
                    cout << "Transmitting: " << endl;
                    cout << dccl_command.DebugString() << endl;
                    dccl_command.Clear();
                    m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(), bytes.size());
                } else {
                    stringstream ss;
                    ss << "shoreside has no command to send for id 1" << endl;
                    cout << ss.str() << endl;
                    m_Comms.Notify("PURSUIT_ERROR", ss.str());
                }
            } else if (slot == 3) {
                if (got_commands[1]) {
                    std::string bytes;
                    codec->encode(&bytes, dccl_command);
                    cout << "Transmitting: " << endl;
                    cout << dccl_command.DebugString() << endl;
                    dccl_command.Clear();
                    m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(), bytes.size());
                } else {
                    stringstream ss;
                    ss << "shoreside has no command to send for id 2" << endl;
                    cout << ss.str() << endl;
                    m_Comms.Notify("PURSUIT_ERROR", ss.str());
                }
            } else if (slot == 6) {
                if (got_commands[2]) {
                    std::string bytes;
                    codec->encode(&bytes, dccl_command);
                    cout << "Transmitting: " << endl;
                    cout << dccl_command.DebugString() << endl;
                    dccl_command.Clear();
                    m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(), bytes.size());
                } else {
                    stringstream ss;
                    ss << "shoreside has no command to send for id 3" << endl;
                    cout << ss.str() << endl;
                    m_Comms.Notify("PURSUIT_ERROR", ss.str());
                }
            }

            // doing receives
            if (!got_receive && (slot==2 || slot==5 || slot==8)) {
                cout << "Missed receive before slot " << slot << endl;
                int id;
                switch (slot) {
                case 2:
                    id = 1;
                    break;
                case 5:
                    id = 2;
                    break;
                case 8:
                    id=3;
                    break;
                }

                stringstream ss;
                ss << id << ":0:0:";
                ss << tdma_engine.getCycleCount();
                m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss.str());
            } else {
                got_receive = false;
            }
        }
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PursuitShore::OnStartUp() {
    return (true);
}

