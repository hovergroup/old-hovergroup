/*
 * pPursuitVehicle
 *        File: PursuitVehicle.h
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#ifndef PursuitVehicle_HEADER
#define PursuitVehicle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class PursuitVehicle : public CMOOSApp
{
 public:
   PursuitVehicle();
   ~PursuitVehicle();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
};

#endif 
