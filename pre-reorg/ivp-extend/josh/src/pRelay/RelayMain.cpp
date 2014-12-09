/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RelayMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "Relay.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "Relay.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "pRelay";
  
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
  Relay RelayApp;

  //run it
  RelayApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

