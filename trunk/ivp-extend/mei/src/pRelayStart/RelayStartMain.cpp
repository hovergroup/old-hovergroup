/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayStartMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "RelayStart.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "RelayStart.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "pRelayStart";
  
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
  RelayStart RelayStartApp;

  //run it
  RelayStartApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

