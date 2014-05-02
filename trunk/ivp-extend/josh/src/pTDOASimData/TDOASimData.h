/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOASimData.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TDOASimData_HEADER
#define TDOASimData_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "tdoa.pb.h"
#include "JoshUtils.h"
#include "goby/acomms/dccl.h"
#include <iostream>
#include <fstream>

enum StateEnum {
    LEADER_SLOT = 0,
    F1_SLOT,
    F2_SLOT,
    F3_SLOT,
    PAUSED
};

class TDOASimData : public CMOOSApp
{
public:
    TDOASimData();
    ~TDOASimData();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();

private:
    double m_targetx, m_targety;
    double m_f1x, m_f1y;
    double m_f2x, m_f2y;
    double m_f3x, m_f3y;
    double m_range1, m_range2, m_range3;
    double m_randomOffset;

    double m_period, m_lastUpdate;

    StateEnum m_state;
    bool m_paused;

    TDOAUpdate m_outgoingUpdate;

    std::ifstream input;

    void postOutput();
    void calculateRanges();
    void addData(double x, double y, int id, double time);
};

#endif 
