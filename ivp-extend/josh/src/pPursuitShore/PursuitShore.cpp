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

        // update our position history
        dccl_report.add_x_history(m_navx);
        dccl_report.add_y_history(m_navy);

        // if our slot, send update
        if (tdma_engine.getCurrentSlot() == m_id) {
            dccl_report.set_id(m_id);
            std::string bytes;
            encoder->encode(&bytes, dccl_report);
            cout << "Transmitting: " << endl;
            cout << dccl_report.DebugString() << endl;
            dccl_report.Clear();
            m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", (void*) bytes.data(), bytes.size());
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
                    ss.str().clear();
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

bool PursuitShore::OnStartUp() {
    return (true);
}

