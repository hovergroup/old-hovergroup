
// Listener Modem Implementation
// 
// Listens for messages to report statistics
//
// Args: usb# modem#
//

#include <iostream>
using namespace std

int main(int argc, char* argv[]){
	
	if(argc < 2){
	cout << "listener usb# modem#"
	return 1;
	}

	usb = "/dev/ttyUSB"+argv[0];
	modem = argv[1];
	
	cout << usb << "/n" << modem
	
	return 0;
}
