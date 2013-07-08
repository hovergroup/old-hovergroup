/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommCmdTransmit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommCmdTransmit_HEADER
#define AcommCmdTransmit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "RemusAMessages.h"

class AcommCmdTransmit : public CMOOSApp
{
 public:
   AcommCmdTransmit();
   ~AcommCmdTransmit();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();

 private: // Configuration variables

 private:
};

#endif 
