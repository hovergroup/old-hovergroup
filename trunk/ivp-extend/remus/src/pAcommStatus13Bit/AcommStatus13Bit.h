/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommStatus13Bit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommStatus13Bit_HEADER
#define AcommStatus13Bit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class AcommStatus13Bit : public CMOOSApp
{
 public:
   AcommStatus13Bit();
   ~AcommStatus13Bit();

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
