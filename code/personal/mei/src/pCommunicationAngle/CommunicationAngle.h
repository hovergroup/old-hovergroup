/*
 * Mei Cheung
 *        File: CommunicationAngle.h
 *  Created on: 
 *      Author: 
 */

#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <math.h>
using namespace std;

class CommunicationAngle : public CMOOSApp
{
public:
    CommunicationAngle();
    ~CommunicationAngle();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();
    void PublishAnswer(bool pathFound, double elev_angle, double transmission_loss);
    bool RegisterCollab();

private: // Configuration variables
    double surface_sound_speed;
    double sound_speed_gradient;
    double water_depth;
    double time_interval;

private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;

    double navx,navy,collabx,collaby;
    double navdepth,navheading,navspeed;
    double collabdepth,collabheading,collabspeed;
    double goto_x,goto_y,goto_d;
    string myname,mycollab;
    double last_post_time;
    bool collabFound;
};

#endif 
