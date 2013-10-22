/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOAShoreside.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TDOAShoreside_HEADER
#define TDOAShoreside_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class TDOAShoreside : public CMOOSApp
{
 public:
   TDOAShoreside();
   ~TDOAShoreside();

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
