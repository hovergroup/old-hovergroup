/*
 * Mei Cheung
 *        File: PrimeFactor.cpp
 *  Created on: 
 *      Author: 
 */

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h"

using namespace std;

const string PrimeFactor::MATLAB_CMD1 =
        "matlab -nodisplay -nosplash -nojvm -r \"['asdf ' num2str(factor(";
const string PrimeFactor::MATLAB_CMD2 = "))], exit\" | grep asdf";

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
    m_inputCount = 0;
    m_outputCount = 0;
}

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        if (key == "NUM_VALUE") {
            try {
                uint64_t input = boost::lexical_cast<uint64_t>(msg.GetString());
                if (input > pow(2,53)) {
                    cout << "Input too large" << endl;
                } else {
                    boost::thread workerThread(boost::bind(&PrimeFactor::factor, this, input, m_inputCount));
                    workerThread.detach();

                    m_inputCount++;
                }
            } catch (const boost::bad_lexical_cast &) {
                cout << "Failed to cast input" << endl;
            }
        }
    }

    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
{
    m_Comms.Register("NUM_VALUE", 0);
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
{
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
{
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
}

void PrimeFactor::factor(uint64_t input, int inputCount) {
    cout << "Input #" << inputCount << " is " << input << endl;
    stringstream cmd_stream;
    cmd_stream << MATLAB_CMD1 << input << MATLAB_CMD2;
    cout << "command: " << cmd_stream.str() << endl;

    string result = exec(cmd_stream.str().c_str());
    if (result.find("asdf") == string::npos) {
        cout << "Error executing calculation" << endl;
        return;
    }
    result = MOOSChomp(result, "\n");
    cout << "result: " << result << endl;

    vector<unsigned long> factors;
    MOOSChomp(result, " ");
    while (!result.empty()) {
        factors.push_back(atoi(MOOSChomp(result, " ").c_str()));
    }
    for (int i=0; i<factors.size(); i++) {
        cout << factors[i] << " ";
    }
    cout << endl;

    outputMutex.lock();
    // do output
    m_outputCount++;
    outputMutex.unlock();
}


std::string PrimeFactor::exec(const char* cmd) {
   FILE* pipe = popen(cmd, "r");
   if (!pipe) return "ERROR";
   char buffer[128];
   std::string result = "";
   while(!feof(pipe)) {
           if(fgets(buffer, 128, pipe) != NULL)
                   result += buffer;
   }
   pclose(pipe);
   return result;
}
