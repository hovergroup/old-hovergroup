#ifndef __SERIAL_H
#define __SERIAL_H

// the other includes needed (such as string, vector, etc.) are apparent already in these three
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <iostream>

class THREAD_CLASS
{
public:
	THREAD_CLASS();
	
	void spawnWorker( std::string msg );
	
private:
	boost::thread worker_thread;
	
	void workerFunction( std::string msg );
};

#endif 
