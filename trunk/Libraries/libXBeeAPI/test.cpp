#include <XBeeAPI.h>
#include <string>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <numeric>
#include <exception>
#include <stdint.h>

	
using namespace std;
using namespace xbee_api;
	

int main() {
	XBeeAPI xbee;
	
	TRANSMIT_REQUEST tr;
	vector<unsigned char> dat;
	dat.push_back(0x01);
	dat.push_back(0x02);
	dat.push_back(0x03);
	tr.data = dat;
	//tr.broadcast_radius = 0xaabbccdd;
	tr.address = 0x1122334455667788;
	
	vector<unsigned char> copy(tr.data.size(), 0);
	memcpy(&copy[0], &tr.data[0], tr.data.size() );
	
	//xbee.transmitRequest( &tr);
		
	// cout << "size of address: " << sizeof(tr.address) << endl;
	// cout << "data size: " << sizeof(tr.data) << " vs. " << tr.data.size() << endl;
	
	// cout << "tr: ";
	// cout << hex << tr.address << "   ";
	// for (int i=0; i<copy.size(); i++) {
		// cout << hex << (int) tr.data[i] << " ";
	// }
	// cout << endl;
	
	// cout << "tr: ";
	// unsigned char *ptr = &tr.placeHolder;
	// for (int i=0; i<sizeof(tr); i++) {
		// cout << hex << (int) *ptr << " ";
		// ptr++;
	// }
	// cout << endl;
	
	xbee.openPort();
	xbee.setBaudRate( 230400 );
	sleep(1);
	//cout << "sending network discover" << endl;
	//xbee.sendNetworkDiscover();
	cout << "sending node discover: QUAD" << endl;
	xbee.sendNodeDiscover("QUAD");
	// cout << "duty cycle: " << dec << xbee.getLocalDutyCycle() << endl;
	//cout << "duty cycle: " << dec << xbee.getLocalDutyCycle() << endl;
	sleep(10000);
	
	// //cout << "Baud rate is " << dec << xbee.getLocalBaudRate() << endl;
	// //sleep(1);
	// //cout << "resetting: " << dec << xbee.softwareReset() << endl;
	// sleep(10000);
	//xbee.closePort();
}
