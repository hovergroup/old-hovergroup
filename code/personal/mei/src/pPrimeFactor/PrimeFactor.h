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

private: // Configuration variables
    class worker_thread{
    public:
    	worker_thread(uint64_t number_in);
    	void operator()();

    private:
    	uint64_t m_Number;
    };

private: // State variables
    unsigned int m_iterations;
    double       m_timewarp;
};

#endif 
