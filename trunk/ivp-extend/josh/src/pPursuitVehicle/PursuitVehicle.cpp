/*
 * pPursuitVehicle
 *        File: PursuitVehicle.cpp
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#include <iterator>
#include "MBUtils.h"
#include "PursuitVehicle.h"

using namespace std;
using namespace JoshUtil;

//---------------------------------------------------------
// Constructor

PursuitVehicle::PursuitVehicle() {
    m_running = false;
    receive_count=0;

    codec = goby::acomms::DCCLCodec::get();

    dccl_report.set_ack(false);
}

//---------------------------------------------------------
// Destructor

PursuitVehicle::~PursuitVehicle() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PursuitVehicle::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == "PURSUIT_COMMAND") {
            string cmd = MOOSToUpper(msg.GetString());
            if (cmd == "RUN") {
                tdma_engine.run();
                command_trajectory.clear();
            } else if (cmd == "STOP") {
                tdma_engine.stop();
            } else if (cmd == "RESET") {
                tdma_engine.reset();
                receive_count = 0;
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
                if (reception.getSource() == 4 && reception.getStatus() == HoverAcomms::GOOD) {

                    // perform decode
                    PursuitCommandDCCL cmd;
                    bool decode_okay = true;
                    try {
                        codec->decode(reception.getData(), &cmd);
                    } catch (goby::acomms::DCCLException &) {
                        stringstream ss;
                        ss << "failed decoding acomms message" << endl;
                        cout << ss.str() << endl;
                        m_Comms.Notify("PURSUIT_ERROR", ss.str());
                        decode_okay = false;
                    }

                    if (decode_okay) {
                        receive_count++;
                        m_Comms.Notify("PURSUIT_RECEIVE_COUNT", (double) receive_count);
                        cout << "Implementing new command trajectory" << endl;
                        command_trajectory.clear();
                        switch (m_id) {
                        case 1:
                            for (int i=0; i<cmd.vehicle1_cmd_size(); i++) {
                                command_trajectory.push_back(cmd.vehicle1_cmd(i));
                            }
                            break;
                        case 2:
                            for (int i=0; i<cmd.vehicle2_cmd_size(); i++) {
                                command_trajectory.push_back(cmd.vehicle2_cmd(i));
                            }
                            break;
                        case 3:
                            for (int i=0; i<cmd.vehicle3_cmd_size(); i++) {
                                command_trajectory.push_back(cmd.vehicle3_cmd(i));
                            }
                            break;
                        }
                        m_Comms.Notify("PURSUIT_TRAJECTORY_LENGTH", command_trajectory.size());
                        dccl_report.set_ack(true);
                    }
                }
            }
        }
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PursuitVehicle::OnConnectToServer() {
    string test_param;
    m_MissionReader.GetConfigurationParam("id", m_id);
    m_MissionReader.GetConfigurationParam("positive_x", positive_x);
    m_MissionReader.GetConfigurationParam("positive_y", positive_y);
    m_MissionReader.GetConfigurationParam("negative_x", negative_x);
    m_MissionReader.GetConfigurationParam("negative_y", negative_y);

    string my_name;
    m_MissionReader.GetValue("Community", my_name);

    XYSegList trackline;
    trackline.add_vertex(positive_x, positive_y, 0, "");
    trackline.add_vertex(negative_x, negative_y, 0, "");
    trackline.set_label(my_name+"_pursuit_trackline");
    m_Comms.Notify("VIEW_SEGLIST", trackline.get_spec());

    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        list<string>::iterator p;
        for (p = sParams.begin(); p != sParams.end(); p++) {
            string original_line = *p;
            string param = stripBlankEnds(toupper(biteString(*p, '=')));
            string value = stripBlankEnds(*p);

            if (param == "CODE") {
                string left = MOOSChomp(value,",");
                int a = atoi(left.c_str());
                double b = atof(value.c_str());
                command_map[a] = b;
                cout << "Added " << a << " --> " << b << " to command map." << endl;
            }
        }
    }

    tdma_engine.parseConfig(m_MissionReader, GetAppName());
    cout << tdma_engine.getSummary() << endl;
    m_Comms.Notify("TDMA_SUMMARY", tdma_engine.getSingleLineSummary());

    m_Comms.Register("PURSUIT_COMMAND", 0);
    m_Comms.Register("NAV_X", 0);
    m_Comms.Register("NAV_Y", 0);
    m_Comms.Register("ACOMMS_RECEIVED", 0);
    return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PursuitVehicle::Iterate() {
    if (tdma_engine.testAdvance()) {
        m_Comms.Notify("TDMA_SLOT", tdma_engine.getCurrentSlot());
        m_Comms.Notify("TDMA_CYCLE_COUNT", tdma_engine.getCycleCount());
        m_Comms.Notify("TDMA_CYCLE_NUMBER", tdma_engine.getCycleNumber());

        // update our position history
        dccl_report.add_slot_history(tdma_engine.getCurrentSlot());
        dccl_report.add_x_history(m_navx);
        dccl_report.add_y_history(m_navy);

        // if our slot, send update
        if (tdma_engine.getCurrentSlot() == m_id) {
            dccl_report.set_id(m_id);

            try {
                codec->validate<PursuitReportDCCL>();
            } catch (goby::acomms::DCCLException& e) {
                std::cout << "failed to validate encoder" << std::endl;
                exit (1);
            }
            std::string bytes;
            codec->encode(&bytes, dccl_report);

            cout << "Transmitting: " << endl;
            cout << dccl_report.DebugString() << endl;
            dccl_report.Clear();
            m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(), bytes.size());
            cout << "Total size " << bytes.size() << endl;
            dccl_report.set_ack(false);
        }

        // implement commands
        if (command_trajectory.size() > 0) {
            if (command_map.find(command_trajectory[0]) != command_map.end()) {
                double desired_speed = command_map[command_trajectory[0]];
                m_Comms.Notify("PURSUIT_DESIRED_SPEED", desired_speed);
                m_Comms.Notify("PURSUIT_QUANTIZED_COMMAND", command_trajectory[0]);
                command_trajectory.erase(command_trajectory.begin());

                stringstream ss;
                if (desired_speed == 0) {
                    m_Comms.Notify("PURSUIT_ACTION", "STATION");
                } else {
                    ss << "speed = " << fabs(desired_speed);
                    m_Comms.Notify("PURSUIT_WAYPOINT_UPDATES", ss.str());
                    ss.str("");
                    ss << "points = ";
                    if ( desired_speed > 0 ) {
                        ss << positive_x << "," << positive_y;
                    } else {
                        ss << negative_x << "," << negative_y;
                    }
                    m_Comms.Notify("PURSUIT_WAYPOINT_UPDATES", ss.str());
                    m_Comms.Notify("PURSUIT_ACTION", "WAYPOINT");
                }

            } else {
                stringstream ss;
                ss << "Value " << command_trajectory[0] << " does not exist in map";
                cout << ss.str() << endl;
                m_Comms.Notify("PURSUIT_ERROR", ss.str());
            }
        } else {
            cout << "Command trajectory empty" << endl;
        }

        m_Comms.Notify("PURSUIT_TRAJECTORY_LENGTH", command_trajectory.size());
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PursuitVehicle::OnStartUp() {
    return (true);
}
