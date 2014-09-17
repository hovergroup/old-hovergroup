/*
 * protobufDCCLExample
 *        File: protobufDCCLExample.cpp
 *  Created on: Sep 10, 2013
 *      Author: Josh Leighton
 */

#include <iostream>
#include "pursuit.pb.h"

#include "goby/acomms/dccl.h"

using namespace std;


int main() {
	double nav_x = 320.5423;
	double nav_y = 50.34;

	PursuitReportDCCL report;
	report.set_id(3);
	report.add_slot_history(2);
	report.add_x_history(543);
	report.add_y_history(-123);
	report.set_ack(true);

	cout << report.DebugString() << endl;

	std::cout << "Without dccl: " << report.SerializeAsString().size() << std::endl;
	goby::acomms::DCCLCodec* codec = goby::acomms::DCCLCodec::get();
	try {
		codec->validate<PursuitReportDCCL>();
	} catch (goby::acomms::DCCLException& e) {
		std::cout << "failed to validate" << std::endl;
	}
	std::string bytes;
	codec->encode(&bytes, report);
	std::cout << "With dccl: " << bytes.size() << std::endl;

	cout << "hex: " << endl;
	for (int i=0; i<bytes.size(); i++) {
	    cout << hex << (int) bytes[i] << " ";
	}
	cout << endl;

	PursuitReportDCCL report2;
	try {
	    codec->decode(bytes, &report2);
	} catch(goby::acomms::DCCLException&) {
        cout << "failed to decode acomms message" << endl;
    }
	cout << report2.DebugString() << endl;
}
