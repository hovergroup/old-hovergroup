/*
 * pTDOAVehicle
 *        File: TDOAVehicle.cpp
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#include <iterator>
#include "MBUtils.h"
#include "TDOAVehicle.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;
using namespace JoshUtil;

//---------------------------------------------------------
// Constructor

TDOAVehicle::TDOAVehicle() {
    m_running = false;
    receive_counts = vector<int>(4,0);

    codec = goby::acomms::DCCLCodec::get();
    try {
        codec->validate<TDOAReportDCCL>();
        codec->validate<TDOACommandDCCL>();
    } catch (goby::acomms::DCCLException& e) {
        std::cout << "failed to validate encoder" << std::endl;
        exit (1);
    }

    got_command = false;
}

//---------------------------------------------------------
// Destructor

TDOAVehicle::~TDOAVehicle() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TDOAVehicle::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == "TDOA_COMMAND") {
            string cmd = MOOSToUpper(msg.GetString());
            if (cmd == "RUN") {
                tdma_engine.run();
                m_Comms.Notify("TDOA_ACTION", "STATION");
            } else if (cmd == "STOP") {
                tdma_engine.stop();
            } else if (cmd == "RESET") {
                tdma_engine.reset();
                receive_counts = vector<int>(3,0);
            }
        } else if (key == "NAV_X") {
            m_navx = msg.GetDouble();
        } else if (key == "NAV_Y") {
            m_navy = msg.GetDouble();
        } else if (key == "ACOMMS_RECEIVED") {
            HoverAcomms::AcommsReception reception;
            // try to parse acomms message
            if (reception.parseFromString(msg.GetString())) {
                // check source and status
                if (reception.getStatus() == HoverAcomms::GOOD) {
                    if (reception.getSource() == 4) {
                        // perform decode
                        TDOACommandDCCL cmd;
                        bool decode_okay = true;
                        try {
                            codec->decode(reception.getData(), &cmd);
                        } catch (goby::acomms::DCCLException &) {
                            stringstream ss;
                            ss << "failed decoding acomms message" << endl;
                            cout << ss.str() << endl;
                            m_Comms.Notify("TDOA_ERROR", ss.str());
                            decode_okay = false;
                        }

                        if (decode_okay) {
                            for (int i=0; i<cmd.data_size(); i++) {
                                if (cmd.data(i).id() == m_id) {
                                    got_command = true;
                                    stringstream ss1;
                                    ss1 << "points = " << cmd.data(i).x() <<
                                            "," << cmd.data(i).y();
                                    m_Comms.Notify("TDOA_WAYPOINT_UPDATES", ss1.str());
                                    m_Comms.Notify("TDOA_ACTION", ss1.str());
                                    m_Comms.Notify("TDOA_WAYPOINT_X", cmd.data(i).x());
                                    m_Comms.Notify("TDOA_WAYPOINT_Y", cmd.data(i).y());
                                    m_Comms.Notify("TDOA_COMMAND_RECEIVED", 1.0);
                                    break;
                                }
                            }
                            receive_counts[0]++;
                            stringstream ss;
                            for (int i=0; i<receive_counts.size(); i++) {
                                if (i!=0)
                                    ss << ",";
                                ss << receive_counts[i];
                            }
                            m_Comms.Notify("TDOA_RECEIVE_COUNTS", ss.str());
                        }
                    } else {
                        TDOAReportDCCL report;
                        bool decode_okay = true;
                        try {
                            codec->decode(reception.getData(), &report);
                        } catch (goby::acomms::DCCLException &) {
                            stringstream ss;
                            ss << "failed decoding acomms message" << endl;
                            cout << ss.str() << endl;
                            m_Comms.Notify("TDOA_ERROR", ss.str());
                            decode_okay = false;
                        }

                        if (decode_okay) {
                            for (int i=0; i<report.data_size(); i++) {
                                bool already_have = false;
                                for (int j=0; j<dccl_report.data_size(); j++) {
                                    if (report.data(i).id() == dccl_report.data(j).id()) {
                                        already_have = true;
                                        break;
                                    }
                                }

                                if (!already_have) {
                                    dccl_report.add_data()->CopyFrom(report.data(i));
                                }
                            }

                            receive_counts[reception.getSource()]++;
                            stringstream ss;
                            for (int i=0; i<receive_counts.size(); i++) {
                                if (i!=0)
                                    ss << ",";
                                ss << receive_counts[i];
                            }
                            m_Comms.Notify("TDOA_RECEIVE_COUNTS", ss.str());
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

bool TDOAVehicle::OnConnectToServer() {

    string test_param;
    m_MissionReader.GetConfigurationParam("id", m_id);

    tdma_engine.parseConfig(m_MissionReader, GetAppName());
    cout << tdma_engine.getSummary() << endl;
    m_Comms.Notify("TDMA_SUMMARY", tdma_engine.getSingleLineSummary());

    m_Comms.Register("TDOA_COMMAND", 0);
    m_Comms.Register("NAV_X", 0);
    m_Comms.Register("NAV_Y", 0);
    m_Comms.Register("ACOMMS_RECEIVED", 0);
    return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOAVehicle::Iterate() {
    if (tdma_engine.testAdvance()) {
        m_Comms.Notify("TDMA_SLOT", tdma_engine.getCurrentSlot());
        m_Comms.Notify("TDMA_CYCLE_COUNT", tdma_engine.getCycleCount());
        m_Comms.Notify("TDMA_CYCLE_NUMBER", tdma_engine.getCycleNumber());

        int slot = tdma_engine.getCurrentSlot();

        if (slot == 0) {
            dccl_report.Clear();
        }

        if (slot == 1) {
            TDOATrio *trio = dccl_report.add_data();
            trio->set_id(m_id);
            trio->set_x(m_navx);
            trio->set_y(m_navy);
        }

        if (slot == m_id+1) {
            std::string bytes;
            codec->encode(&bytes, dccl_report);
            cout << "Transmitting: " << endl;
            cout << dccl_report.DebugString() << endl;

            dccl_report.Clear();
            m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(),
                    bytes.size());
            cout << "Total size " << bytes.size() << endl;
        }
    }
    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TDOAVehicle::OnStartUp() {
    return (true);
}
