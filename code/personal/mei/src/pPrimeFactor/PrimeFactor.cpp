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

//For using matlab
//const string PrimeFactor::MATLAB_CMD1 =
//		"matlab -nodisplay -nosplash -nojvm -r \"['asdf ' num2str(factor(";
//const string PrimeFactor::MATLAB_CMD2 = "))], exit\" | grep asdf";

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
					//Spawn a thread and detach it ~Good bye!
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
	double start_time = MOOSTime();

	//These lines are for using matlab to factor- Replaced
	//    cout << "Input #" << inputCount << " is " << input << endl;
	//stringstream cmd_stream;
	//cmd_stream << MATLAB_CMD1 << input << MATLAB_CMD2;
	//    cout << "command: " << cmd_stream.str() << endl;

	//string result = exec(cmd_stream.str().c_str());
	//if (result.find("asdf") == string::npos) {
	//	cout << "Error executing calculation" << endl;
	//	return;
	//}
	//result = MOOSChomp(result, "\n");
	//    cout << "result: " << result << endl;

	//vector<unsigned long> factors;
	//MOOSChomp(result, " ");
	//try{
	//	while (!result.empty()) {
	//		string next = MOOSChomp(result, " ");
	//		if (!next.empty()) {
	//			factors.push_back(boost::lexical_cast<unsigned long>(next));
	//		}
	//	}
	//} catch(const boost::bad_lexical_cast &){
	//	cout << "Error lexical cast factors" << endl;
	//	return;
	//}
	//stringstream factorsOut;
	//for (int i=0; i<factors.size()-1; i++) {
	//	factorsOut << factors[i] << ":";
	//}
	//factorsOut << factors.back();

	//Pollard-rho Factorization
	stringstream factorsOut;
	uint64_t xi = 2;
	uint64_t x2i = 2;
	uint64_t xiPrime = 0;
	uint64_t x2iPrime =0;
	uint64_t factoring = input;
	uint64_t h=1;

	while (factoring>1 && h!=factoring){
		cout << "Factoring: " << factoring << endl;
		h = 1;
		while (h==1){
			xiPrime = xi*xi+1;
			x2iPrime = pow(x2i*x2i+1,2)+1;

			cout << "Primes: "<<xiPrime <<"," <<x2iPrime<< endl;
			
			xi = xiPrime % factoring;
			x2i = x2iPrime % factoring;

			cout << "x's: "<<xi <<"," <<x2i<< endl;
			cout << (xi-x2i) << endl;
			
			uint64_t gcda = abs(xi-x2i);
			uint64_t gcdb = factoring; //this will be the gcd output
			uint64_t gcdc = 0;

			cout << "Finding gcd: " << gcda << "," << gcdb << endl;
			//Find gcd(xi-x2i,input)
			while ( gcda != 0 ) {gcdc = gcda; gcda = gcdb%gcda;  gcdb = gcdc;}
			
			h=gcdb;
			cout << "Found gcd: " << gcdb << endl;
			
			if((h!=1)&&(h!=factoring)){
				factoring /= h;
				if (factoring !=1){ factorsOut << h << ":";}
				else {factorsOut << h;}
				
				cout<<"Found factor: "<< h << " Current divisor: " <<factoring << endl;
			}
		}
	}

	//Thread-safe Output Counting
	outputMutex.lock();
	int outputCount = m_outputCount;
	m_outputCount++;
	outputMutex.unlock();

	//Elapsed time counting
	double elapsed_time = MOOSTime() - start_time;

	stringstream msgOut;
	msgOut << "orig=" << input << ",received=" << inputCount <<
			",calculated=" << outputCount << ",solve_time=" << elapsed_time << ",primes=" <<  factorsOut.str()
			<< ",username=mc2922";

	//Post the solution
	cout << msgOut.str() << endl;
	m_Comms.Notify("PRIME_RESULT", msgOut.str());
}

//This function is for using Matlab system commands to factor (The easy way)
//std::string PrimeFactor::exec(const char* cmd) {
//	FILE* pipe = popen(cmd, "r");
//	if (!pipe) return "ERROR";
//	char buffer[128];
//	std::string result = "";
//	while(!feof(pipe)) {
//		if(fgets(buffer, 128, pipe) != NULL)
//			result += buffer;
//	}
//	pclose(pipe);
//	return result;
//}
