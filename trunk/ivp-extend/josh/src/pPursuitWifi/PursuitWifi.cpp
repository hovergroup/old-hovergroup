/*
 * pPursuitWifi
 *        File: PursuitWifi.cpp
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#include <iterator>
#include "MBUtils.h"
#include "PursuitWifi.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;
using namespace JoshUtil;

//---------------------------------------------------------
// Constructor

PursuitWifi::PursuitWifi() {
    m_running = false;

    m_navx = vector<double>(3,0);
    m_navy = vector<double>(3,0);
    m_waypointx = vector<double>(3,0);
    m_waypointy = vector<double>(3,0);
    m_initialx = vector<double>(3,0);
    m_initialy = vector<double>(3,0);
    positive_x = vector<double>(3,0);
    positive_y = vector<double>(3,0);
    negative_x = vector<double>(3,0);
    negative_y = vector<double>(3,0);
    desired_speeds = vector<double>(3,0);


    names.push_back("NOSTROMO");
    names.push_back("SILVANA");
    names.push_back("KESTREL");

    m_project_time = 7;
    m_min_speed = 0.5;

}

//---------------------------------------------------------
// Destructor

PursuitWifi::~PursuitWifi() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PursuitWifi::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == "PURSUIT_COMMAND") {
            string cmd = MOOSToUpper(msg.GetString());
            if (cmd == "RUN") {
                tdma_engine.run();
                m_waypointx = m_initialx;
                m_waypointy = m_initialy;
                m_Comms.Notify("PURSUIT_ACTION_ALL", "STATION");
                for (int i=0; i<3; i++) {
                    stringstream var;
                    var << "PURSUIT_STATION_UPDATES_" << names[i];
                    stringstream val;
                    val << "station_pt = " << m_waypointx[i] << ","
                            << m_waypointy[i];
                    m_Comms.Notify(var.str(), val.str());
                }
            } else if (cmd == "STOP") {
                tdma_engine.stop();
            } else if (cmd == "RESET") {
                tdma_engine.reset();
            }
        } else if (key == "NAV_X_NOSTROMO") {
            m_navx[0] = msg.GetDouble();
        } else if (key == "NAV_Y_NOSTROMO") {
            m_navy[0] = msg.GetDouble();
        } else if (key == "NAV_X_SILVANA") {
            m_navx[1] = msg.GetDouble();
        } else if (key == "NAV_Y_SILVANA") {
            m_navy[1] = msg.GetDouble();
        } else if (key == "NAV_X_KESTREL") {
            m_navx[2] = msg.GetDouble();
        } else if (key == "NAV_Y_KESTREL") {
            m_navy[2] = msg.GetDouble();
        } else if (key == "PURSUIT_VEHICLE_COMMAND" && tdma_engine.getRunState() == TDMAEngine::RUNNING) {
            string line = msg.GetString() + ":";
            for (int i=0; i<3; i++) {
                int id = atoi(MOOSChomp(line, ",").c_str());
                double speed = atof(MOOSChomp(line,":").c_str());
                desired_speeds[id-1] = speed/m_project_time;
                cout << "Command: id " << id << "   speed " << desired_speeds[id-1] << endl;
            }
            runCommands();
        }
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PursuitWifi::OnConnectToServer() {
    m_MissionReader.GetConfigurationParam("positive_x_nostromo", positive_x[0]);
    m_MissionReader.GetConfigurationParam("positive_y_nostromo", positive_y[0]);
    m_MissionReader.GetConfigurationParam("negative_x_nostromo", negative_x[0]);
    m_MissionReader.GetConfigurationParam("negative_y_nostromo", negative_y[0]);
    m_MissionReader.GetConfigurationParam("initial_x_nostromo", m_initialx[0]);
    m_MissionReader.GetConfigurationParam("initial_y_nostromo", m_initialy[0]);

    m_MissionReader.GetConfigurationParam("positive_x_silvana", positive_x[1]);
    m_MissionReader.GetConfigurationParam("positive_y_silvana", positive_y[1]);
    m_MissionReader.GetConfigurationParam("negative_x_silvana", negative_x[1]);
    m_MissionReader.GetConfigurationParam("negative_y_silvana", negative_y[1]);
    m_MissionReader.GetConfigurationParam("initial_x_silvana", m_initialx[1]);
    m_MissionReader.GetConfigurationParam("initial_y_silvana", m_initialy[1]);

    m_MissionReader.GetConfigurationParam("positive_x_kestrel", positive_x[2]);
    m_MissionReader.GetConfigurationParam("positive_y_kestrel", positive_y[2]);
    m_MissionReader.GetConfigurationParam("negative_x_kestrel", negative_x[2]);
    m_MissionReader.GetConfigurationParam("negative_y_kestrel", negative_y[2]);
    m_MissionReader.GetConfigurationParam("initial_x_kestrel", m_initialx[2]);
    m_MissionReader.GetConfigurationParam("initial_y_kestrel", m_initialy[2]);

    m_MissionReader.GetConfigurationParam("project_time", m_project_time);
    m_MissionReader.GetConfigurationParam("min_speed", m_min_speed);

    for (int i=0; i<3; i++) {
        XYSegList trackline;
        trackline.add_vertex(positive_x[i], positive_y[i], 0, "");
        trackline.add_vertex(negative_x[i], negative_y[i], 0, "");
        trackline.set_label(names[i]+"_pursuit_trackline");
        m_Comms.Notify("VIEW_SEGLIST", trackline.get_spec());
    }

    tdma_engine.parseConfig(m_MissionReader, GetAppName());
    cout << tdma_engine.getSummary() << endl;
    m_Comms.Notify("TDMA_SUMMARY", tdma_engine.getSingleLineSummary());

    m_Comms.Register("PURSUIT_COMMAND", 0);
    m_Comms.Register("NAV_X_NOSTROMO", 0);
    m_Comms.Register("NAV_Y_NOSTROMO", 0);
    m_Comms.Register("NAV_X_SILVANA", 0);
    m_Comms.Register("NAV_Y_SILVANA", 0);
    m_Comms.Register("NAV_X_KESTREL", 0);
    m_Comms.Register("NAV_Y_KESTREL", 0);
    m_Comms.Register("PURSUIT_VEHICLE_COMMAND", 0);
    return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PursuitWifi::Iterate() {
    if (tdma_engine.testAdvance()) {
        m_Comms.Notify("TDMA_SLOT", tdma_engine.getCurrentSlot());
        m_Comms.Notify("TDMA_CYCLE_COUNT", tdma_engine.getCycleCount());
        m_Comms.Notify("TDMA_CYCLE_NUMBER", tdma_engine.getCycleNumber());

        stringstream report;
        for (int i=0; i<3; i++) {
            if (i!=0)
                report << ":";
            report << i+1 << ",";
            report << m_navx[i] << ",";
            report << m_navy[i];
        }
        m_Comms.Notify("PURSUIT_VEHICLE_REPORT", report.str());
    }
    return (true);
}

void PursuitWifi::runCommands() {
    for (int i=0; i<3; i++) {
        cout << "Calculating command for id #" << i+1 << endl;
        double desired_speed = desired_speeds[i];
        double current_x = m_waypointx[i];
        double current_y = m_waypointy[i];
        if (desired_speed != 0) {
            // project current target onto the trackline
            double nx, ny;
            perpSegIntPt(positive_x[i], positive_y[i], negative_x[i],
                    negative_y[i], current_x, current_y, nx, ny);
            XYPoint perp_pt(nx, ny);

            double angle, dist;
            double distance = fabs(desired_speed * m_project_time);
            if (desired_speed > 0) {
                angle = relAng(negative_x[i], negative_y[i], positive_x[i],
                        positive_y[i]);
                dist = distPointToPoint(nx, ny, positive_x[i], positive_y[i]);
            } else {
                angle = relAng(positive_x[i], positive_y[i], negative_x[i],
                        negative_y[i]);
                dist = distPointToPoint(nx, ny, negative_x[i], negative_y[i]);
            }
            double targx, targy;
            if (current_x == positive_x[i] && current_y == positive_y[i] && desired_speed > 0) {
                targx = positive_x[i];
                targy = positive_y[i];
                cout << "capped at " << targx << "," << targy
                        << endl;
            } else if (current_x == negative_x[i] && current_y == negative_y[i] && desired_speed < 0) {
                targx = negative_x[i];
                targy = negative_y[i];
                cout << "capped at " << targx << "," << targy
                        << endl;
            } else if (distance > dist) {
                if (desired_speed > 0) {
                    targx = positive_x[i];
                    targy = positive_y[i];
                } else {
                    targx = negative_x[i];
                    targy = negative_y[i];
                }
                cout << "capped at " << targx << "," << targy
                        << endl;
            } else {
                projectPoint(angle, distance, nx, ny, targx, targy);
                cout << "projected to " << targx << "," << targy
                        << endl;
            }
            m_waypointx[i] = targx;
            m_waypointy[i] = targy;

            stringstream ss;
            ss << targx << "," << targy;
            m_Comms.Notify("PURSUIT_WAYPOINT_" + names[i], ss.str());

            ss.str("");
            ss << "points = ";
            if (desired_speed > 0) {
                ss << negative_x[i] << "," << negative_y[i] << ":"
                        << targx << "," << targy;
            } else {
                ss << positive_x[i] << "," << positive_y[i] << ":"
                        << targx << "," << targy;
            }
            m_Comms.Notify("PURSUIT_WAYPOINT_UPDATES_" + names[i], ss.str());
            m_Comms.Notify("PURSUIT_ACTION_" + names[i], "WAYPOINT");

            ss.str("");
            if (fabs(desired_speed) < m_min_speed) {
                desired_speed = m_min_speed;
            }
            ss << "speed = " << fabs(desired_speed);
            m_Comms.Notify("PURSUIT_WAYPOINT_UPDATES_" + names[i], ss.str());

            ss.str("");
            ss << "station_pt = " << targx << "," << targy;
            m_Comms.Notify("PURSUIT_STATION_UPDATES_" + names[i], ss.str());
        }
    }
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PursuitWifi::OnStartUp() {
    return (true);
}
