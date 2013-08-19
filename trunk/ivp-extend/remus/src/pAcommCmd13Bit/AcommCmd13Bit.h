/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommCmd13Bit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommCmd13Bit_HEADER
#define AcommCmd13Bit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class AcommCmd13Bit : public CMOOSApp
{
 public:
   AcommCmd13Bit();
   ~AcommCmd13Bit();

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
