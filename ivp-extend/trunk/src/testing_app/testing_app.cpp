/*
 * testing_app.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

#include <iostream>
//#include <acomms_messages.h>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>

using namespace std;
//using namespace boost::posix_time;
//using namespace lib_acomms_messages;

int main() {

	int num_states = 10;
	int discount_factor = 0.9;
	gsl_matrix * transitions = gsl_matrix_calloc(num_states,num_states);
	gsl_matrix * C = gsl_matrix_calloc(num_states,1);
	gsl_matrix_add_constant(C,1);

	for(int i=0;i<num_states;i++){
		gsl_matrix * Am = gsl_matrix_calloc(num_states,num_states);
		gsl_matrix_set_identity(Am);
		gsl_matrix * tempB = gsl_matrix_alloc(num_states,num_states);
		gsl_matrix_memcpy(tempB,transitions);
		gsl_matrix_scale(tempB,discount_factor);
		gsl_matrix_sub(Am,tempB);
		gsl_permutation * p = gsl_permutation_alloc(num_states);
		int s;
		gsl_linalg_LU_decomp(Am,p,&s);
		gsl_linalg_LU_svx(Am,p,)

		gsl_matrix_free(Am);
		gsl_matrix_free(tempB);
		gsl_permutation_free(num_states);
	}

	gsl_matrix_free(transitions);
	gsl_matrix_free(C);

	//	string date_string = "191112";
//	string time_string = "225446.123";
//	string modified_date = "20"+date_string.substr(4,2)+
//			date_string.substr(2,2)+
//			date_string.substr(0,2);
//	string composite = modified_date+"T"+time_string;
//
//	cout << composite << endl;
//
//	ptime t(from_iso_string(composite));
//
//	cout << to_simple_string(t) << endl;
//
//	time_duration td = t.time_of_day();
//
//	cout << to_simple_string( td ) << endl;
//
//	double seconds = td.total_milliseconds()/1000.0;
//
//	cout << seconds << endl;

//	cout << to_simple_string(time_from_string(to_simple_string(t))) << endl;
//	SIMPLIFIED_RECEIVE_INFO info;
//	info.vehicle_name = "bob";
//	info.num_frames = 3;
//	info.num_good_frames = 2;
//	info.num_bad_frames = 1;
//	info.rate = 2;
//
//	cout << info.serializeToString() << endl;
//
//	SIMPLIFIED_RECEIVE_INFO info2( info.serializeToString() );
//
//	cout << info2.serializeToString() << endl;

	return 0;
}
