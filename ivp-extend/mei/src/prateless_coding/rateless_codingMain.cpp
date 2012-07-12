/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: rateless_codingMain.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "rateless_coding.h"

using namespace std;

int main(int argc, char *argv[])
{
  // default parameters file
  string sMissionFile = "rateless_coding.moos";
        
  //under what name shoud the application register with the MOOSDB?
  string sMOOSName = "prateless_coding";
  
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
  rateless_coding rateless_codingApp;

  //run it
  rateless_codingApp.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}

