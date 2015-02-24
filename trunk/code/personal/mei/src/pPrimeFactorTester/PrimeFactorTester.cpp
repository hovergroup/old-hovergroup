/*
 * Mei Cheung
 *        File: PrimeFactorTester.cpp
 *  Created on: 
 *      Author: 
 */

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactorTester.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactorTester::PrimeFactorTester()
{
    m_iterations = 0;
    m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

PrimeFactorTester::~PrimeFactorTester()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactorTester::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;

        string key = msg.GetKey();
		if (key == "PRIME_RESULT") {
			string result=msg.GetString();
			stringstream msgOut;
			msgOut<<result;

			MOOSChomp(result,"="); // discard "input="
			uint64_t input = boost::lexical_cast<uint64_t>(MOOSChomp(result,","));
			MOOSChomp(result,",primes=");
			string primes_str = MOOSChomp(result,",");

			uint64_t product = 1;
			while (!primes_str.empty()) {
				string next = MOOSChomp(primes_str,":");
				product *= boost::lexical_cast<uint64_t>(next);
			}

			cout << "Input: " << input << " ,Output: " << product << endl;
			if (product==input){
				//cout << "Factors valid!" << endl;
				msgOut << ",valid=true";
			}else{
				msgOut << ",valid=false";
			}

			m_Comms.Notify("PRIME_RESULT_VALID",msgOut.str());
		}

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    }

    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactorTester::OnConnectToServer()
{
    // register for variables here
    // possibly look at the mission file?
    // m_MissionReader.GetConfigurationParam("Name", <string>);
    // m_Comms.Register("VARNAME", 0);

	m_Comms.Register("PRIME_RESULT", 0);
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactorTester::Iterate()
{
    m_iterations++;
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactorTester::OnStartUp()
{
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        list<string>::iterator p;
        for(p=sParams.begin(); p!=sParams.end(); p++) {
            string original_line = *p;
            string param = stripBlankEnds(toupper(biteString(*p, '=')));
            string value = stripBlankEnds(*p);
            
            if(param == "FOO") {
                //handled
            } else if(param == "BAR") {
                //handled
            }
        }
    }

    m_timewarp = GetMOOSTimeWarp();

    RegisterVariables();	
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactorTester::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
}

