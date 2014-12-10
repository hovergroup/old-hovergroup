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
    sent_reports = vector<bool>(3,false);

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
            if (!multicast) {
                ClearCommands();
            }

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
                dccl_command.set_has_1(true);
                break;
            case 2:
                dccl_command.clear_vehicle2_cmd();
                for (int i=0; i<trajectory.size(); i++) {
                    dccl_command.add_vehicle2_cmd(trajectory[i]);
                }
                dccl_command.set_has_2(true);
                break;
            case 3:
                dccl_command.clear_vehicle3_cmd();
                for (int i=0; i<trajectory.size(); i++) {
                    dccl_command.add_vehicle3_cmd(trajectory[i]);
                }
                dccl_command.set_has_3(true);
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
                        codec->decode(reception.getData(), &report);
                    } catch (goby::acomms::DCCLException &) {
                        stringstream ss;
                        ss << "shoreside failed decoding acomms message" << endl;
                        cout << ss.str() << endl;
                        m_Comms.Notify("PURSUIT_ERROR", ss.str());
                        decode_okay = false;
                    }

                    // output info if good receive and haven't already posted report for this id
                    if (decode_okay) {
                        if (!sent_reports[report.id()-1]) {
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
                            ss << ":" << report.slot();
                            ss << "," << report.x();
                            ss << "," << report.y();
                            m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss.str());

                            got_receive = true;
                            sent_reports[report.id()-1] = true;
                        } else {
                            stringstream ss;
                            ss << "Already made report for id " << report.id()-1 << " when trying to post good report.";
                            m_Comms.Notify("PURSUIT_ERROR", ss.str());
                        }
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

        int slot = tdma_engine.getCurrentSlot();

        if (slot == 0) {
            sent_reports = vector<bool>(3,false);
        }

        if (multicast) {
            if (slot == 5) {
                ClearCommands();
            }

            if (slot == 1) {
                stringstream ss1;
                ss1 << "-1:-1:-1:";
                ss1 << tdma_engine.getCycleCount();
                m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss1.str());
            }

            // if our slot, send update
            if (slot == 0) {
                bool got_command = false;
                // transmit as long as we have at least one
                for (int i=0; i<got_commands.size(); i++) {
                    if (got_commands[i] == true) {
                        got_command = true;
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

            if (!got_receive && (slot==3 || slot==5 || slot==7)) {
                cout << "Missed receive before slot " << slot << endl;
                int id;
                switch (slot) {
                case 3:
                    id = 1;
                    break;
                case 5:
                    id = 2;
                    break;
                case 7:
                    id=3;
                    break;
                }

                if (!sent_reports[id-1]) {
                    sent_reports[id-1] = true;

                    stringstream ss;
                    ss << id << ":0:0:";
                    ss << tdma_engine.getCycleCount();
                    m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss.str());
                } else {
                    stringstream ss;
                    ss << "Already made report for id " << id << " when trying to post fail report.";
                    m_Comms.Notify("PURSUIT_ERROR", ss.str());
                }
            } else {
                got_receive = false;
            }
        } else { // interleaved



            // clear command buffers during report slots
            if (slot==2 || slot==6 || slot==10) {
                ClearCommands();
            }

            // post dummy report during placeholder slots
            if (slot==1 || slot==5 || slot==9) {
               stringstream ss1;
               ss1 << "-1:-1:-1:";
               ss1 << tdma_engine.getCycleCount();
               m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss1.str());
            }

            // if command slot, send update
            if (slot == 0) {
                if (got_commands[2]) {
                    dccl_command.set_has_3(true);
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
            } else if (slot == 4) {
                if (got_commands[0]) {
                    dccl_command.set_has_1(true);
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
            } else if (slot == 8) {
                if (got_commands[1]) {
                    dccl_command.set_has_2(true);
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
            }

            // doing receives
            if (!got_receive && (slot==3 || slot==7 || slot==11)) {
                cout << "Missed receive before slot " << slot << endl;
                int id;
                switch (slot) {
                case 3:
                    id = 1;
                    break;
                case 7:
                    id = 2;
                    break;
                case 11:
                    id=3;
                    break;
                }

                if (!sent_reports[id-1]) {
                    sent_reports[id-1] = true;

                    stringstream ss;
                    ss << id << ":0:0:";
                    ss << tdma_engine.getCycleCount();
                    m_Comms.Notify("PURSUIT_VEHICLE_REPORT", ss.str());
                } else {
                    stringstream ss;
                    ss << "Already made report for id " << id << " when trying to post fail report.";
                    m_Comms.Notify("PURSUIT_ERROR", ss.str());
                }
            } else {
                got_receive = false;
            }
        }
    }

    return (true);
}

void PursuitShore::ClearCommands() {
    got_commands = vector<bool>(3,false);
    dccl_command.Clear();
    cout << "clearing command buffer" << endl;
    dccl_command.set_has_1(false);
    dccl_command.set_has_2(false);
    dccl_command.set_has_3(false);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PursuitShore::OnStartUp() {
    return (true);
}

