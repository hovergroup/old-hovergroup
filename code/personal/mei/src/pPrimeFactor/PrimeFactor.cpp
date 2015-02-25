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
				unsigned long long int input = boost::lexical_cast<unsigned long long int>(msg.GetString());
				if (input > pow(2,48)) {
					cout << "Input number greater than 2^48." << endl;
				} else {
					//Spawn a thread and detach it ~Good bye!
					boost::thread workerThread(boost::bind(&PrimeFactor::factor, this, input, m_inputCount));
					workerThread.detach();

					m_inputCount++;
				}
			} catch (const boost::bad_lexical_cast &) {
				cout << "Failed to cast input." << endl;
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

void PrimeFactor::factor(unsigned long long int input, int inputCount) {
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

	stringstream factorsOut, trialFactors;
	unsigned long long int prefactored = input;

	//Pre factor small primes - Pollard-rho may miss these
	int prefactors [10] = {2,3,5,7,11,13,17,19,23,29};
	int prefactor_counter = 0;
	while(prefactor_counter<10){
		if(prefactored % prefactors[prefactor_counter]==0){
			prefactored /= prefactors[prefactor_counter];
			if(factorsOut.tellp()>0){factorsOut<<":";}
			factorsOut << prefactors[prefactor_counter];
		}
		else{
			prefactor_counter++;
		}
	}

	cout<<"Prefactoring concluded: "<<factorsOut.str()<<endl;

	if(prefactored!=1){
		//Pollard-rho Integer Factorization (+used to determine primeness)
		bool factors_found = false;
		int trial_counter = 0;
		int max_trial_counter = 10;	//Try a few times with new offset
		srand(time(NULL));

		while (trial_counter<max_trial_counter && !factors_found){
			trialFactors.str("");
			unsigned long long int xi = 2;
			unsigned long long int x2i = 2;
			unsigned long long int xiPrime = 0;
			unsigned long long int x2iPrime = 0;
			unsigned long long int factoring = prefactored;
			unsigned long long int h=1;
			unsigned long long int offset = 1 + (rand() % 50);	//this is arbitrary
			unsigned long long int internal_counter = 0;
			unsigned long long int internal_max_counter = sqrt(input);	//Probably factored after this many trials or number is prime

			//cout << "The offset was: " << offset << endl;
			//cout << "Iterating on: "<<prefactored<<endl;

			while ((internal_counter < internal_max_counter) && factoring!=1){	//Pollard-rho is correct in probability
				xiPrime = xi*xi+offset;
				x2iPrime = pow(x2i*x2i+offset,2)+offset;

				xi = xiPrime % factoring;
				x2i = x2iPrime % factoring;

				unsigned long long int gcda;
				if(xi>=x2i){gcda = xi-x2i;}
				else if(x2i>xi){gcda = x2i-xi;}
				unsigned long long int gcdb = factoring; //this will be the gcd output
				unsigned long long int gcdc = 0;

				//cout << "Finding gcd: " << gcda << "," << gcdb << endl;
				while ( gcda != 0 ) {gcdc = gcda; gcda = gcdb%gcda;  gcdb = gcdc;}

				h=gcdb;
				//cout << "Found gcd: " << gcdb << endl;

				if(h!=1){
					factoring /= h;
					if (factoring!=1){factors_found=true;}
					if (factorsOut.tellp()>0){trialFactors << ":";}
					trialFactors << h;

					//cout<<"Found factor: "<< h << " Current divisor: " <<factoring << endl;
				}
				internal_counter++;
				if (internal_counter==internal_max_counter){ //Ended with factoring being Prime
					if (factorsOut.tellp()>0){trialFactors << ":";}
					trialFactors << factoring;
				}
			}
			//if (!factors_found){cout << "Failed to find prime factors: " << trial_counter << " times so far." << endl;}
			trial_counter++;
		}
	}
	factorsOut << trialFactors.str();

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

//This function is for using Matlab system commands to factor (The lazy way)
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
