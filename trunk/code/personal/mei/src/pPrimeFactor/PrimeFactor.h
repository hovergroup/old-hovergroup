/*
 * Mei Cheung
 *        File: PrimeFactor.h
 *  Created on: 
 *      Author: 
 */

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdint.h>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <math.h>

class PrimeFactor : public CMOOSApp
{
public:
    PrimeFactor();
    ~PrimeFactor();

protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    void RegisterVariables();

private:
    //static const std::string MATLAB_CMD1, MATLAB_CMD2;
    int m_inputCount, m_outputCount;

    //std::string exec(const char* cmd); //For matlab use
    void factor(unsigned long long int input, int inputCount);
    boost::mutex outputMutex;
};

#endif
