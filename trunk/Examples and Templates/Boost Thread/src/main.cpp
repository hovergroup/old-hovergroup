#include "thread_class.h"

using namespace std;

int main()
{
	THREAD_CLASS t;
	t.spawnWorker( "worker 1" );
	
	sleep(2);
}
