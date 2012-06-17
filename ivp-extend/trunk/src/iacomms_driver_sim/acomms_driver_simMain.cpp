/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_driver_simMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "acomms_driver_sim.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "acomms_driver_sim.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "iacomms_driver_sim";
  
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
  acomms_driver_sim acomms_driver_simApp;

  //run it
  acomms_driver_simApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

