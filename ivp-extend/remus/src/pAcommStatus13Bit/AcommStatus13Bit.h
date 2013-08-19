/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommStatus13Bit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommStatus13Bit_HEADER
#define AcommStatus13Bit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include "RemusAMessages.h"

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

   double m_period, m_offset;
       double m_vx, m_vy; //vehicle status (x,y,depth,bearing,speed)
       double m_osx_minimum, m_osx_maximum;
       double m_osy_minimum, m_osy_maximum;

       int m_lastSentSlot;

       bool enabled;

       double getTime();
       int getNextSlot();
       double getSlotTime(int slot);

       void post();
       void getStatusString13Bits();


 private: // Configuration variables

 private: // State variables
};

#endif 
