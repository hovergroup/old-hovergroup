#include <iostream>
#include <gsl/gsl_blas.h>
#include <vector>
#include <stdio.h>
#include "goby/acomms/protobuf/mm_driver.pb.h"
#include "goby/acomms/acomms_helpers.h"
#include "goby/acomms/acomms_constants.h"
#include "goby/acomms/protobuf/modem_message.pb.h"
#include "goby/acomms/protobuf/driver_base.pb.h"
#include "MOOSLib.h"

using namespace std;

int main () {

//	string sNMEA = "$GPRMC,183923.50,A,4221.51338,N,07105.25272,W,0.010,,151012,,,D*6D\r\n";
	string sNMEA = "$GPRMC,144354.50,A,4221.51534,N,07105.25329,W,0.479,,161012,,,A*64";


    unsigned char xCheckSum=0;

    string sToCheck;
    MOOSChomp(sNMEA,"$");
    sToCheck = MOOSChomp(sNMEA,"*");
    string sRxCheckSum = sNMEA;

    //now calculate what we think check sum should be...
    string::iterator p;
    for(p = sToCheck.begin();p!=sToCheck.end();p++)
    {
        xCheckSum^=*p;
    }

    ostringstream os;

    os.flags(ios::hex);
    os<<(int)xCheckSum;//<<ends;
    string sExpected = os.str();

    if (sExpected.length() < 2)
    {
        sExpected = "0" + sExpected;
    }

    ///now compare to what we recived..

    cout << MOOSStrCmp(sExpected,sRxCheckSum) << endl;
}

///*
// * testing_app.cpp
// *
// *  Created on: May 24, 2012
// *      Author: josh
// */
//
//#include <fstream>
////#include <sstream>
//////#include <acomms_messages.h>
////#include <boost/date_time/posix_time/posix_time.hpp>
//#include <gsl/gsl_statistics_double.h>
//#include <gsl/gsl_matrix.h>
//#include <gsl/gsl_blas.h>
//#include <gsl/gsl_permutation.h>
//#include <gsl/gsl_linalg.h>
//#include <vector>
//#include <map>
//////#include "XYSegList.h"
//#include <iostream>
//#include <string.h>
//#include <vector>
//	gsl_matrix *temp_matrix;
//	temp_matrix = gsl_matrix_calloc(2,2);
//	gsl_matrix *temp_matrix_2;
//	temp_matrix_2 = gsl_matrix_calloc(2,2);
//	gsl_matrix_set(temp_matrix_2,0,0,2);
//	gsl_matrix_set(temp_matrix_2,0,1,8);
//	gsl_matrix_set(temp_matrix_2,1,0,-4);
//	gsl_matrix_set(temp_matrix_2,1,1,9);
//
//	int s;
//	gsl_permutation *p = gsl_permutation_alloc (2);
//	gsl_linalg_LU_decomp (temp_matrix_2,p, &s);
//	gsl_linalg_LU_invert(temp_matrix_2,p,temp_matrix);
//	gsl_matrix_fprintf(stdout, temp_matrix, "%f");
//#include <math.h>
//#include <string>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sstream>
//#include <boost/tokenizer.hpp>
//
//using namespace std;
////using namespace boost::posix_time;
//////using namespace lib_acomms_messages;
////
////string string_chomp( string s, char c ) {
////	for ( int i=0; i<s.size()-1; i++ ) {
////		if ( s[i]==c ) return s.substr(i+1, s.size()-i+1);
////	}
////	return s;
////}
//
//// our structure for sending data
//struct MY_DATA {
//	unsigned char packet_id;
//	int latitude;
//	int longitude;
//	unsigned short heading;
//};
//
//int main() {
//
//	gsl_matrix *temp_matrix;
//	temp_matrix = gsl_matrix_calloc(2,2);
//	gsl_matrix *temp_matrix_2;
//	temp_matrix_2 = gsl_matrix_calloc(2,2);
//	gsl_matrix_set(temp_matrix_2,0,0,2);
//	gsl_matrix_set(temp_matrix_2,0,1,8);
//	gsl_matrix_set(temp_matrix_2,1,0,-4);
//	gsl_matrix_set(temp_matrix_2,1,1,9);
//
//	int s;
//	gsl_permutation *p = gsl_permutation_alloc (2);
//	gsl_linalg_LU_decomp (temp_matrix_2,p, &s);
//	gsl_linalg_LU_invert(temp_matrix_2,p,temp_matrix);
//	gsl_matrix_fprintf(stdout, temp_matrix, "%f");
//
////	FILE* f=fopen("sysMatrices_2Hz_forMOOSKF.txt","r");
////	gsl_matrix *A,*B,*B_noise, *B_in, *C,*Q,*R; 	//matrix inputs
////
////	A = gsl_matrix_alloc(4,4);
////	B = gsl_matrix_alloc(4,1);
////	B_noise = gsl_matrix_alloc(4,4);
////	B_in = gsl_matrix_alloc(4,4);
////	C = gsl_matrix_alloc(2,4);
////	Q = gsl_matrix_alloc(4,4);
////	R = gsl_matrix_alloc(2,2);
////
////	gsl_matrix_fscanf(f,A);
////	cout << "Read A, ";
////	  gsl_matrix_fprintf(stdout,A,"%f");
////	gsl_matrix_fscanf(f,B);
////	cout << "B, ";
////	  gsl_matrix_fprintf(stdout,B,"%f");
////	gsl_matrix_fscanf(f,B_noise);
////	cout << "B_noise, ";
////	  gsl_matrix_fprintf(stdout,B_noise,"%f");
////	gsl_matrix_fscanf(f,B_in);
////	cout << "B_in, ";
////	  gsl_matrix_fprintf(stdout,B_in,"%f");
////	gsl_matrix_fscanf(f,C);
////	cout << "C, ";
////	  gsl_matrix_fprintf(stdout,C,"%f");
////	gsl_matrix_fscanf(f,Q);
////	cout << "Q, ";
////	  gsl_matrix_fprintf(stdout,Q,"%f");
////	gsl_matrix_fscanf(f,R);
////	cout << "R --> ";
////			  gsl_matrix_fprintf(stdout,R,"%f");
////	cout << "End Reading"<< endl;
////
////	fclose(f);
////
////		cout<<"Reading Points"<<endl;
////		//time, desired heading,x1,y1,x2,y2
////		vector<double> wpx,wpy,time,headings;
////
////		while(waypointsfile.good()){
////			getline(waypointsfile,one_point);
////			int pos = one_point.find(',');
////
////			if(pos>0){
////
////				stringstream ss;
////				char discard;
////				double param;
////
////				ss.str(one_point);
////				ss >> param; time.push_back(param); ss >> discard;
////				ss >> param; headings.push_back(param); ss >> discard;
////				ss >> param; wpx.push_back(param); ss >> discard;
////				ss >> param; wpy.push_back(param); ss >> discard;
////
////				ss.str("");
////				ss<<"type=gateway,x="<<wpx[total_points]<<
////						",y="<<wpx[total_points]<<",SCALE=4.3,label="<<total_points<<",COLOR=red,width=4.5";
////				//m_Comms.Notify("VIEW_MARKER",ss.str());
////				cout << ss.str() << endl;
////				total_points++;
////			}
////		}
////
////		cout<<"Read "<<total_points<<" points."<<std::endl;
//
////	gsl_matrix * mm,*m2;
////	mm = gsl_matrix_alloc(4,3);
////	m2 = gsl_matrix_alloc(4,3);
////	FILE* f=fopen("test.txt","r");
////	cout << "file" << endl;
////	gsl_matrix_fscanf(f,mm);
////	gsl_matrix_fscanf(f,m2);
////	fclose(f);
////cout << "read" << endl;
////	  /* print matrix the easy way */
////	  printf("Matrix mm\n");
////	  gsl_matrix_fprintf(stdout,mm,"%f");
////	  gsl_matrix_fprintf(stdout,m2,"%f");
//
//
////	string message = "$C83.2R483";
////
////	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
////	vector<string> subs;
////
////	boost::char_separator<char> sep("$CR");
////	tokenizer tok(message, sep);
////	for ( tokenizer::iterator beg=tok.begin();
////			beg!=tok.end(); ++beg ) {
////		subs.push_back(*beg);
////	}
////	cout << atof(subs[0].c_str()) << endl;
////	cout << subs[1] << endl;
//}
//
////	// fill a struct with data
////	MY_DATA data_to_send;
////	data_to_send.packet_id = 0x01;
////	data_to_send.latitude = (int) (42.358456 * pow(10,7));
////	data_to_send.longitude = (int) (-71.087589 * pow(10,7));
////	data_to_send.heading = 185;
////
////	// construct our array/vector and fill it with data
////	vector<unsigned char> packet (sizeof(data_to_send), 0);
////	memcpy(&packet[0], &data_to_send.packet_id, sizeof(data_to_send));
////
////	// to get our data out, do the reverse
////	MY_DATA data_received;
////	memcpy(&data_received.packet_id, &packet[0], sizeof(data_to_send));
////
////	// print out the data
////	cout << "packet id: 0x" << hex << (int) data_received.packet_id << dec << endl;
////	cout << "lat: " << data_received.latitude / pow(10,7) << endl;
////	cout << "lon: " << data_received.longitude / pow(10,7) << endl;
////	cout << "heading: " << data_received.heading << endl;
////
////	string s( (char*) &packet[0], packet.size() );
////
////	return 0;
//
////
////	unsigned char data[] = {0x01, 0x10};
////
////	// method 1 - bitwise operations
////	unsigned short result1 = (data[0]<<8) + data[1];
////	cout << result1 << endl;
////
////	// method 2 - memcpy
////	unsigned short result2;
////	memcpy(&result2, &data[0],2);
////	cout << result2 << endl;
//
////
////	fstream filestr;
////
////	filestr.open ("Experiment1_July13_10OriPcks.txt", fstream::in | fstream::out | fstream::app);
////
////	// >> i/o operations here <<
//////int data;
////	//for(int j=0;j<3;j++){
//////int counter = 0;
////	for(int j=0;j<21;j++){
////		for(int i=0;i<32;i++){
////			int	data;
////			filestr >> data;
////			cout << (char) data;
////		}
////		cout <<endl;
////		cout << endl;
////	}
////
//////		cout << data << endl;
//////	}
////
////	filestr.close();
//
//	//DO NOT DELETE
//	//	int num_states = 10;
//	//	int discount_factor = 0.9;
//	//	gsl_matrix * transitions = gsl_matrix_calloc(num_states,num_states);
//	//	gsl_matrix * C = gsl_matrix_calloc(num_states,1);
//	//	gsl_matrix_add_constant(C,1);
//	//
//	//	for(int i=0;i<num_states;i++){
//	//		gsl_matrix * Am = gsl_matrix_calloc(num_states,num_states);
//	//		gsl_matrix_set_identity(Am);
//	//		gsl_matrix * tempB = gsl_matrix_alloc(num_states,num_states);
//	//		gsl_matrix_memcpy(tempB,transitions);
//	//		gsl_matrix_scale(tempB,discount_factor);
//	//		gsl_matrix_sub(Am,tempB);
//	//		gsl_permutation * p = gsl_permutation_alloc(num_states);
//	//		int s;
//	//		gsl_linalg_LU_decomp(Am,p,&s);
//	////		gsl_linalg_LU_svx(Am,p,)
//	//
//	//		gsl_matrix_free(Am);
//	//		gsl_matrix_free(tempB);
//	////		gsl_permutation_free(num_states);
//	//	}
//	//
//	//	gsl_matrix_free(transitions);
//	//	gsl_matrix_free(C);
//
////	return 0;
////}
