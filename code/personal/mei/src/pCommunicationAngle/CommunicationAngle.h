/*
 * Mei Cheung
 *        File: CommunicationAngle.h
 *  Created on: 
 *      Author: 
 */

#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

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

private: // Configuration variables

private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
};

#endif 
