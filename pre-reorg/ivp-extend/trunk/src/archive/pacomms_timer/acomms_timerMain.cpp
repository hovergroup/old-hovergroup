/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_timerMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "acomms_timer.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "acomms_timer.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "pacomms_timer";
  
  switch(argc)
    {
    case 3:
      //command line says don't register with default name              
      sMOOSName = argv[2];
    case 2:
      //command line says don't use default config file
      sMissionFile = argv[1];
    }
  
  //make an application
  acomms_timer acomms_timerApp;

  //run it
  acomms_timerApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

