/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ScheduledTransmitMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "ScheduledTransmit.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "ScheduledTransmit.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "pScheduledTransmit";
  
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
  ScheduledTransmit ScheduledTransmitApp;

  //run it
  ScheduledTransmitApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

