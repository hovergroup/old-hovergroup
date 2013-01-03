/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Tulip26bitMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "Tulip26bit.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "Tulip26bit.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "pTulip26bit";
  
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
  Tulip26bit Tulip26bitApp;

  //run it
  Tulip26bitApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

