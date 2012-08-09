/*
 * testing_app.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <fstream>
//#include <sstream>
////#include <acomms_messages.h>
//#include <boost/date_time/posix_time/posix_time.hpp>
////#include <gsl/gsl_statistics_double.h>
////#include <gsl/gsl_matrix.h>
////#include <gsl/gsl_blas.h>
////#include <gsl/gsl_permutation.h>
////#include <gsl/gsl_linalg.h>
//#include <vector>
////#include "XYSegList.h"
//
//using namespace std;
//using namespace boost::posix_time;
////using namespace lib_acomms_messages;
//
//string string_chomp( string s, char c ) {
//	for ( int i=0; i<s.size()-1; i++ ) {
//		if ( s[i]==c ) return s.substr(i+1, s.size()-i+1);
//	}
//	return s;
//}

#include <iostream>
#include <string.h>
#include <vector>
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <boost/tokenizer.hpp>

using namespace std;

// our structure for sending data
struct MY_DATA {
	unsigned char packet_id;
	int latitude;
	int longitude;
	unsigned short heading;
};

int main() {

	string message = "$C83.2R483";

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	vector<string> subs;

	boost::char_separator<char> sep("$CR");
	tokenizer tok(message, sep);
	for ( tokenizer::iterator beg=tok.begin();
			beg!=tok.end(); ++beg ) {
		subs.push_back(*beg);
	}
	cout << atof(subs[0].c_str()) << endl;
	cout << subs[1] << endl;
}

//	// fill a struct with data
//	MY_DATA data_to_send;
//	data_to_send.packet_id = 0x01;
//	data_to_send.latitude = (int) (42.358456 * pow(10,7));
//	data_to_send.longitude = (int) (-71.087589 * pow(10,7));
//	data_to_send.heading = 185;
//
//	// construct our array/vector and fill it with data
//	vector<unsigned char> packet (sizeof(data_to_send), 0);
//	memcpy(&packet[0], &data_to_send.packet_id, sizeof(data_to_send));
//
//	// to get our data out, do the reverse
//	MY_DATA data_received;
//	memcpy(&data_received.packet_id, &packet[0], sizeof(data_to_send));
//
//	// print out the data
//	cout << "packet id: 0x" << hex << (int) data_received.packet_id << dec << endl;
//	cout << "lat: " << data_received.latitude / pow(10,7) << endl;
//	cout << "lon: " << data_received.longitude / pow(10,7) << endl;
//	cout << "heading: " << data_received.heading << endl;
//
//	string s( (char*) &packet[0], packet.size() );
//
//	return 0;

//
//	unsigned char data[] = {0x01, 0x10};
//
//	// method 1 - bitwise operations
//	unsigned short result1 = (data[0]<<8) + data[1];
//	cout << result1 << endl;
//
//	// method 2 - memcpy
//	unsigned short result2;
//	memcpy(&result2, &data[0],2);
//	cout << result2 << endl;

//
//	fstream filestr;
//
//	filestr.open ("Experiment1_July13_10OriPcks.txt", fstream::in | fstream::out | fstream::app);
//
//	// >> i/o operations here <<
////int data;
//	//for(int j=0;j<3;j++){
////int counter = 0;
//	for(int j=0;j<21;j++){
//		for(int i=0;i<32;i++){
//			int	data;
//			filestr >> data;
//			cout << (char) data;
//		}
//		cout <<endl;
//		cout << endl;
//	}
//
////		cout << data << endl;
////	}
//
//	filestr.close();
	//	int x, y;
	//	string lat, lon;
	//	string s = "lat,102:lon,-5";
	//	s = string_chomp( s, ',' );
	//	stringstream ss(s);
	//	ss >> x;
	//	s = string_chomp( s, ',' );
	//	ss.str(s);
	//	ss >> y;
	//	cout << x << " " << y << endl;
	//	double normal_indices[18] = {10.141,1.1656,0.6193,0.4478,0.359,0.3035,0.2645,
	//			0.2353,0.2123,0.1109,0.0761,0.0582,0.0472,0.0397,0.0343,0.0302,0.0269,0.0244};
	//	double gindex;
	//	for(int num_obs=1;num_obs<100;num_obs++){
	//		if(num_obs<=10){
	//			gindex = normal_indices[num_obs-2];//table starts from 2 obs, vector starts from 0 index
	//			std::cout<<num_obs<<": "<<gindex<<std::endl;
	//		}
	//		else{	//interpolate
	//			int base = 7+(num_obs/10);
	//			int offset = num_obs%10;
	//			double difference = (normal_indices[base+1]-normal_indices[base])/10;
	//			gindex = normal_indices[base] + offset*difference;
	//			//std::cout<<"Base: "<<normal_indices[base]<<endl;
	//			std::cout<<num_obs<<": "<<gindex<<std::endl;
	//		}
	//	}
	//	string time_string = "2012-06-11T20:16:24.800000";
	//	string time_string = "2002-Jan-01 10:00:01.123456789";
	//	ptime p = time_from_string(time_string);
	//	cout << to_simple_string(p) << endl;

	//	XYSegList seglist;
	//
	//	std::string filename = "relay_waypoints.txt";
	//	std::string one_point;
	//	std::ifstream waypointsfile("relay_waypoints.txt",std::ifstream::in);
	//
	//	while(waypointsfile.good()){
	//				getline(waypointsfile,one_point);
	//				int pos = one_point.find(',');
	//	if(pos>=0){
	//				std::string subx = one_point.substr(0,pos-1);
	//				std::string suby = one_point.substr(pos+1);
	//				seglist.add_vertex(atof(subx.c_str()),atof(suby.c_str()));
	//			}
	//	}
	//
	//	for (unsigned i=0; i<seglist.size() ; i++){
	//	    cout << seglist.get_vx(i)<<",";
	//	    cout << seglist.get_vy(i)<<endl;
	//	}

	//DO NOT DELETE
	//	int num_states = 10;
	//	int discount_factor = 0.9;
	//	gsl_matrix * transitions = gsl_matrix_calloc(num_states,num_states);
	//	gsl_matrix * C = gsl_matrix_calloc(num_states,1);
	//	gsl_matrix_add_constant(C,1);
	//
	//	for(int i=0;i<num_states;i++){
	//		gsl_matrix * Am = gsl_matrix_calloc(num_states,num_states);
	//		gsl_matrix_set_identity(Am);
	//		gsl_matrix * tempB = gsl_matrix_alloc(num_states,num_states);
	//		gsl_matrix_memcpy(tempB,transitions);
	//		gsl_matrix_scale(tempB,discount_factor);
	//		gsl_matrix_sub(Am,tempB);
	//		gsl_permutation * p = gsl_permutation_alloc(num_states);
	//		int s;
	//		gsl_linalg_LU_decomp(Am,p,&s);
	////		gsl_linalg_LU_svx(Am,p,)
	//
	//		gsl_matrix_free(Am);
	//		gsl_matrix_free(tempB);
	////		gsl_permutation_free(num_states);
	//	}
	//
	//	gsl_matrix_free(transitions);
	//	gsl_matrix_free(C);

//	return 0;
//}
