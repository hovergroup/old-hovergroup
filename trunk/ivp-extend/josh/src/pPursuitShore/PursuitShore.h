/*
 * pPursuitShore
 *        File: PursuitShore.h
 *  Created on: Sept 5, 2014
 *      Author: Josh Leighton
 */

#ifndef PursuitShore_HEADER
#define PursuitShore_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class PursuitShore : public CMOOSApp
{
 public:
   PursuitShore();
   ~PursuitShore();

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
