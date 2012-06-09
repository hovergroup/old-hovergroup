/*
 * testing_app.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

#include <iostream>
#include <fstream>
//#include <acomms_messages.h>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <vector>
#include "XYSegList.h"

using namespace std;
//using namespace boost::posix_time;
//using namespace lib_acomms_messages;

int main() {

	XYSegList seglist;

	std::string filename = "relay_waypoints.txt";
	std::string one_point;
	std::ifstream waypointsfile("relay_waypoints.txt",std::ifstream::in);

	while(waypointsfile.good()){
				getline(waypointsfile,one_point);
				int pos = one_point.find(',');
	if(pos>=0){
				std::string subx = one_point.substr(0,pos-1);
				std::string suby = one_point.substr(pos+1);
				seglist.add_vertex(atof(subx.c_str()),atof(suby.c_str()));
			}
	}

	for (unsigned i=0; i<seglist.size() ; i++){
	    cout << seglist.get_vx(i)<<",";
	    cout << seglist.get_vy(i)<<endl;
	}

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

	return 0;
}
