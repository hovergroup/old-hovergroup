#include "thread_class.h"

using namespace std;

THREAD_CLASS::THREAD_CLASS() {

}

void THREAD_CLASS::spawnWorker( string msg ) {
	boost::bind(&msg, this);
	// worker_thread = boost::thread(boost::bind(&THREAD_CLASS::workerFunction, this), 
		// boost::bind(msg, this) );
}

void THREAD_CLASS::workerFunction( string msg ) {
	cout << msg << endl;
}