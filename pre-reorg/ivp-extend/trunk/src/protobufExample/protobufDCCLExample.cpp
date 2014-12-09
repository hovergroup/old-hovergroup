/*
 * protobufDCCLExample
 *        File: protobufDCCLExample.cpp
 *  Created on: Sep 10, 2013
 *      Author: Josh Leighton
 */

#include <iostream>
#include "dcclExample.pb.h"

#include "goby/acomms/dccl.h"

using namespace std;


int main() {
	double nav_x = 320.5423;
	double nav_y = 50.34;

//	VehicleStatus vs;
	VehicleStatusDCCL vsd;
//	for (int i=0; i<36; i++) {
//	    vsd.add_vel_command(5);
//	}

//
//	vs.set_nav_x(nav_x);
//	vs.set_nav_y(nav_y);
//	vs.set_health(VehicleStatus_HealthEnum_HEALTH_GOOD);
//
//	vsd.set_nav_x(nav_x);
//	vsd.set_nav_y(nav_y);
//	vsd.set_health(VehicleStatusDCCL_HealthEnum_HEALTH_GOOD);

	cout << vsd.DebugString() << endl;

	std::cout << "Without dccl: " << vsd.SerializeAsString().size() << std::endl;
	goby::acomms::DCCLCodec* codec = goby::acomms::DCCLCodec::get();
	try {
		codec->validate<VehicleStatusDCCL>();
	} catch (goby::acomms::DCCLException& e) {
		std::cout << "failed to validate" << std::endl;
	}
	std::string bytes;
	codec->encode(&bytes, vsd);
	std::cout << "With dccl: " << bytes.size() << std::endl;
}
