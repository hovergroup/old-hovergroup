/*
 * Mei Cheung
 *        File: PrimeFactorTester.h
 *  Created on: 
 *      Author: 
 */

#ifndef PrimeFactorTester_HEADER
#define PrimeFactorTester_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdint.h>
#include <boost/lexical_cast.hpp>

class PrimeFactorTester : public CMOOSApp
{
public:
    PrimeFactorTester();
    ~PrimeFactorTester();

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
