/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommCmd13Bit.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef AcommCmd13Bit_HEADER
#define AcommCmd13Bit_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "RemusAMessages.h"

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

   double m_osx_minimum, m_osx_maximum;
   double m_osy_minimum, m_osy_maximum;

   void decodeStatusString13Bits(const std::string data);


 private: // Configuration variables

 private: // State variables
};

#endif 
