/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: EricMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Eric.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "Eric.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "pEric";
  
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
  Eric EricApp;

  //run it
  EricApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

