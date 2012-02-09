#include "serial.h"

using namespace std;

int main()
{
	SERIAL_EXAMPLE serial;
	serial.open_port("/dev/ttyUSB0", 9600);
	sleep(5);
	
	string s = "Hello world.";
	vector<unsigned char> v( s.begin(), s.end() );
	serial.writeData( &v[0], v.size() );
	sleep(5);
	
	serial.close_port();
}
