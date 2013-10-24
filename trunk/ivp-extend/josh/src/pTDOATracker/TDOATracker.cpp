/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOATracker.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "TDOATracker.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TDOATracker::TDOATracker()
{
    acomms_heard = vector<bool>(3,0);
    GetHermite("matrices.txt");
}

//---------------------------------------------------------
// Destructor

TDOATracker::~TDOATracker()
{
    gsl_matrix_free(s1);
    gsl_matrix_free(s2);
    gsl_matrix_free(s3);
    gsl_matrix_free(s4);
    gsl_matrix_free(s5);
    gsl_matrix_free(s6);
    gsl_matrix_free(s7);
    gsl_matrix_free(s8);
    gsl_matrix_free(s9);
    gsl_vector_free(w);


}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TDOATracker::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();

        if(key== "TDOA_PROTOBUF"){
            TDOA_protobuf.ParseFromString(msg.GetString());
            vector<int> slots_heard = vector<int>(3,0);
            switch (TDOA_protobuf.cycle_state()){
            case 0:
                if(TDOA_protobuf.data_size()>0){
                    slots_heard(1) = 1;
                }
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                acomms_heard = vector<bool>(3,0);
                break;
            }
            NotifyStatus(TDOA_protobuf.cycle_state(),slots_heard);
        }
    }

    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TDOATracker::OnConnectToServer()
{
    // register for variables here
    // possibly look at the mission file?
    // m_MissionReader.GetConfigurationParam("Name", <string>);
    // m_Comms.Register("VARNAME", 0);

    m_MissionReader.GetConfigurationParam("TDOAID",tdoa_id);
    m_MissionReader.GetConfigurationParam("XOffset",x_offset);
    m_MissionReader.GetConfigurationParam("YOffset",y_offset);
    m_MissionReader.GetConfigurationParam("SDim",s_dim);

    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOATracker::Iterate()
{
    return(true);
}

void TDOATracker::GetPriors(gsl_vector * xhat, gsl_matrix * P){
    for(int i=0;i<s_dim;i++){
        for(int j=0;j<s_dim;j++){
            for(int k=0;k<s_dim;k++){
                MatrixSquareRoot(s_dim,P);

            }
        }
    }

}

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
    gsl_matrix *evec_real = gsl_matrix_alloc(dim,dim);

    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
          gsl_matrix_set(evec_real,i,j,GSL_REAL(gsl_matrix_complex_get(evec, i, j)));
        }
    }

    gsl_vector_complex_free(eval);
    gsl_matrix_complex_free(evec);

    gsl_matrix *sqrt_e = gsl_matrix_alloc(dim,dim);
    for(int i=0;i<dim;i++){
        gsl_matrix_set(sqrt_e,i,i,sqrt(gsl_vector_get(eval_real,i)));
    }

    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,evec_real,sqrt_e,0.0,matrix_in);

    gsl_matrix * evec_inv = gsl_matrix_alloc(dim,dim);
    gsl_matrix * matrix_out = gsl_matrix_alloc(dim,dim);
    gsl_linalg_LU_decomp (evec_real, perm, &s);
    gsl_linalg_LU_invert (evec_real, perm, evec_inv);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,matrix_in,evec_inv,0.0,matrix_out);

    return matrix_out;
}

void TDOATracker::NotifyStatus(int cycle_id, vector<int> message_ids){
    stringstream tellme;
    tellme.str("Cycle:");
    tellme << cycle_id << ' ';
    tellme << "Heard";
    for (vector<int>::iterator it = message_ids.begin() ; it != message_ids.end(); ++it){
        tellme << *it << ' ';
    }
    m_Comms.Notify("TRACKER_STATUS",tellme.str());
}

void TDOATracker::GetHermite(string txtfile){
    FILE* f = fopen(txtfile.c_str(),"r");

    s1 = gsl_matrix_alloc(s_dim,s_dim);
    s2 = gsl_matrix_alloc(s_dim,s_dim);
    s3 = gsl_matrix_alloc(s_dim,s_dim);
    s4 = gsl_matrix_alloc(s_dim,s_dim);
    s5 = gsl_matrix_alloc(s_dim,s_dim);
    s6 = gsl_matrix_alloc(s_dim,s_dim);
    s7 = gsl_matrix_alloc(s_dim,s_dim);
    s8 = gsl_matrix_alloc(s_dim,s_dim);
    s9 = gsl_matrix_alloc(s_dim,s_dim);

    w = gsl_vector_alloc(s_dim);

    gsl_matrix_fscanf(f,s1);
    gsl_matrix_fscanf(f,s2);
    gsl_matrix_fscanf(f,s3);
    gsl_matrix_fscanf(f,s4);
    gsl_matrix_fscanf(f,s5);
    gsl_matrix_fscanf(f,s6);
    gsl_matrix_fscanf(f,s7);
    gsl_matrix_fscanf(f,s8);
    gsl_matrix_fscanf(f,s9);

    gsl_vector_fscanf(f,w);
    fscanf (f, "%d", &vol);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TDOATracker::OnStartUp()
{
    return(true);
}
