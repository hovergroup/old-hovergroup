#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <stdio.h>

using namespace std;

gsl_matrix* MatrixSquareRoot(int dim, gsl_matrix * matrix_in){

    gsl_permutation * perm = gsl_permutation_alloc(dim);
    int s;
    gsl_vector_complex *eval = gsl_vector_complex_alloc(dim);
    gsl_matrix_complex *evec = gsl_matrix_complex_alloc(dim, dim);

    gsl_eigen_nonsymmv_workspace *w = gsl_eigen_nonsymmv_alloc(dim);

    gsl_eigen_nonsymmv(matrix_in, eval, evec, w);
    gsl_eigen_nonsymmv_free(w);
    gsl_eigen_nonsymmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_DESC);

    gsl_vector_view eval_view = gsl_vector_complex_real(eval);
    gsl_vector *eval_real = &eval_view.vector;

    for(int i=0;i<dim;i++){
        cout<<gsl_vector_get(eval_real,i)<<endl;
    }

    gsl_matrix *evec_real = gsl_matrix_alloc(dim,dim);

    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
          gsl_matrix_set(evec_real,i,j,GSL_REAL(gsl_matrix_complex_get(evec, i, j)));
        }
    }

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 2; j++){
          cout << gsl_matrix_get (evec_real, i, j)<<endl;
        }
    }
    gsl_matrix *sqrt_e = gsl_matrix_alloc(dim,dim);
    for(int i=0;i<dim;i++){
        gsl_matrix_set(sqrt_e,i,i,sqrt(gsl_vector_get(eval_real,i)));
    }

    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,evec_real,sqrt_e,0.0,matrix_in);

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 2; j++){
          cout << gsl_matrix_get (matrix_in, i, j)<<endl;
        }
    }

    gsl_matrix * evec_inv = gsl_matrix_alloc(dim,dim);
    gsl_matrix * matrix_out = gsl_matrix_alloc(dim,dim);
    gsl_linalg_LU_decomp (evec_real, perm, &s);
    gsl_linalg_LU_invert (evec_real, perm, evec_inv);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,matrix_in,evec_inv,0.0,matrix_out);


    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 2; j++){
          cout << gsl_matrix_get (evec_inv, i, j)<<endl;
        }
    }
    return matrix_out;
}

unsigned char LinearEncode( double val, double min, double max, int bits ) {
	unsigned char transmit_val;
	if ( val <= min ) return 0;
	else if ( val >= max ) return pow(2,bits)-1;
	else {
		double ratio = (val-min)/(max-min);
		double scaled = ratio * (pow(2,bits)-1);
		return floor( scaled + .5 );
	}
}

double LinearDecode( unsigned char val, double min, double max, int bits ) {
	double ratio = val / ( pow(2.0,bits) - 1.0 );
	return min + ratio * ( max - min );
}

unsigned char FlexibleEncode(double val,
        std::vector<double> & range_divs, int bits) {

    if (range_divs.size() != pow(2, bits) - 1) {
        std::stringstream ss;
        ss << "Specified number of bits (" << bits;
        ss << ") does not match vector size (" << range_divs.size();
        ss << ")";
        cout << ss.str() << endl;
//        handleDebug(ss.str());
        return 0x00;
    }

    unsigned char transmit_val;

    for (int i = 0; i < range_divs.size() - 1; i++) {
        if ( val < range_divs[i] )
            return i;
    }
    return range_divs.size();
}

int main () {
	int s_dim = 3;
	std::vector<gsl_matrix*> sigma_points (3*s_dim);
	for(int i=0;i<3*s_dim;i++){
		sigma_points[i] = gsl_matrix_alloc(s_dim, s_dim);
	    }

	gsl_vector *w = gsl_vector_alloc(s_dim);
	double vol;

	string txtfile = "HermiteMatrices.txt";
	FILE* f = fopen(txtfile.c_str(),"r");
	for(int i=0;i<3*s_dim;i++){
		cout << "scanning\n";
		gsl_matrix *A = gsl_matrix_alloc(s_dim, s_dim);
		gsl_matrix_fscanf(f,A);

		for(int j=0;j<s_dim;j++){
			for(int k=0;k<s_dim;k++){
				cout << gsl_matrix_get(A,j,k);
			}
			cout << "\n";
		}
		cout << "\n";

	}
	gsl_vector_fscanf(f,w);
	fscanf (f, "%lf", &vol);

	for(int j=0;j<s_dim;j++){
		cout << gsl_vector_get(w,j);
	}
	cout << "\n";
	cout << vol;

//    double data[] = { 7.0  , 10.0,
//                       15.0, 22.0 };
//
//    gsl_matrix_view m = gsl_matrix_view_array(data, 2, 2);
//
//    for (int i = 0; i < 2; i++){
//        for (int j = 0; j < 2; j++){
//          cout << gsl_matrix_get (&m.matrix, i, j)<<endl;;
//        }
//    }
//
//    gsl_matrix* matrix_out = MatrixSquareRoot(2,&m.matrix);
//
//    for (int i = 0; i < 2; i++){
//        for (int j = 0; j < 2; j++){
//          cout << gsl_matrix_get (matrix_out, i, j)<<endl;;
//        }
//    }
//    unsigned char a = 22;
//    unsigned char b = 4;
//    cout << hex << (int) ( (a<<3) + b ) << endl;

//    std::vector<double> range_divs;
//    for ( int i=2; i<9; i++ ) {
//        range_divs.push_back(i*10);
//    }
//
//    unsigned char range =FlexibleEncode(81,range_divs,3);
//    range+=0xf8;
//
//    cout << (int) (range & 0x07)<< endl;

//	double val = 310;
//	unsigned char sent = LinearEncode( val, 0, 310, 5 );
//	cout << (int) sent << endl;
//	double received = LinearDecode( sent, 0, 310, 5);
//	cout << received << endl;
//	cout << (int) LinearEncode( val, 0, 310, 5) << endl;
//	cout << hex << (int) LinearEncode( val, 0, 310, 5) << endl;
//	cout << hex << (int) (LinearEncode( val, 0, 310, 5)<<3) << endl;
}
