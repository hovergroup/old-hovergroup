/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommsSimulator.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommsSimulator_HEADER
#define AcommsSimulator_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class AcommsSimulator: public CMOOSApp {
public:
    AcommsSimulator();
    ~AcommsSimulator();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();

private:
    // Configuration variables

private:
    // State variables
    unsigned int m_iterations;
    double m_timewarp;
};

#endif 
