/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommsController.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "AcommsController.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AcommsController::AcommsController() {
	active = false;
	m_target_x = 0;
	m_target_y = 0;
}

//---------------------------------------------------------
// Destructor

AcommsController::~AcommsController() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AcommsController::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key = msg.GetKey();
		if ( key=="COMPASS_HEADING_UNFILITERED" ) {
			m_unfiltered_heading = msg.GetDouble();
		} else if ( key=="COMPASS_HEADING_FILTERED" ) {
			m_filtered_heading = msg.GetDouble();
		} else if ( key=="GPS_X" ) {
			m_gps_x = msg.GetDouble();
		} else if ( key=="GPS_Y" ) {
			m_gps_y = msg.GetDouble();
		} else if ( key=="GPS_SPEED" ) {
			m_gps_speed = msg.GetDouble();
		} else if ( key=="MISSION_MDOE" ) {
			if ( active && msg.GetString()!="ACOMMS_CONTROL" )
				onInactive();
			else if ( !active && msg.GetString()=="ACOMMS_CONTROL" )
				onActive();
		} else if ( key=="ACOMMS_CONTROLLER_TARGET" ) {
			parseWaypoint( msg.GetString() );
		} else if ( key=="ACOMMS_RECEIVED_DATA" ) {
			received_data = msg.GetString();
		} else if ( key=="ACOMMS_BAD_FRAMES" ) {
			last_receive_time = MOOSTime();
			if ( msg.GetString()=="-1" ) {
				publishWarning("got good receive");
				onReceive( received_data );
			} else {
				publishWarning("got bad receive");
				onLoss();
			}
		} else if ( key=="ACOMMS_DRIVER_STATUS" ) {
			driver_status = msg.GetString();
		}
	}

	return (true);
}

// read your matrices from the moos config file here
void AcommsController::readMatrices() {
	// example matrix reading
	string matrix_string;
	m_MissionReader.GetConfigurationParam("matrix_A", matrix_string);
	matrix_A = generateMatrix( matrix_string );

	m_MissionReader.GetConfigurationParam("matrix_B", matrix_string);
	matrix_B = generateMatrix( matrix_string );

	// example matrix printout
	cout << "Matrix a: " << matrix_A->size1 << " x " << matrix_A->size2 <<  endl;
	for ( int i=0; i<matrix_A->size1; i++ ) {
		for ( int j=0; j<matrix_A->size2; j++ ) {
			cout << gsl_matrix_get(matrix_A,i,j) << " ";
		}
		cout << endl;
	}

	// example matrix multiplication
	gsl_matrix *matrix_C = gsl_matrix_calloc(matrix_A->size1, matrix_B->size2);
	gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, matrix_A, matrix_B, 1.0, matrix_C );
	cout << "Matrix c: " << matrix_C->size1 << " x " << matrix_C->size2 <<  endl;
	for ( int i=0; i<matrix_C->size1; i++ ) {
		for ( int j=0; j<matrix_C->size2; j++ ) {
			cout << gsl_matrix_get(matrix_C,i,j) << " ";
		}
		cout << endl;
	}

}

// called when data is received
void AcommsController::onReceive( string data ) {
	// your code here
}

// called when data lost
void AcommsController::onLoss() {
	// your code here
}

// called on transmit period
void AcommsController::onTransmit() {
	// your code here
}

// when enabled by setting mission_mode
void AcommsController::onActive() {
	active = true;
	last_transmit_time = MOOSTime();
	last_receive_time = MOOSTime()+20;
	signal_sync_loss = false;

	// your code here
}

// when disabled by setting mission_mode
void AcommsController::onInactive() {
	active = false;
	// your code here
}

gsl_matrix * AcommsController::generateMatrix( std::string msg ) {
	gsl_matrix *matrix;
	vector<string> rows = tokenize(msg, ";");
	vector<vector<string> > values;
	for ( int i=0; i<rows.size(); i++ ) {
		values.push_back( tokenize(rows[i],",") );
	}

	int num_rows = rows.size();
	int num_cols = values[0].size();

	matrix = gsl_matrix_calloc( num_rows, num_cols );
	for ( int i=0; i<num_rows; i++ ){
		for ( int j=0; j<num_cols; j++ ) {
			gsl_matrix_set( matrix, i, j, atof(values[i][j].c_str()) );
		}
	}

	return matrix;
}

vector<string> AcommsController::tokenize( string msg, string tokens ) {
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	vector<string> subs;
	boost::char_separator<char> sep(tokens.c_str());
	tokenizer tok(msg, sep);
	for ( tokenizer::iterator beg=tok.begin();
			beg!=tok.end(); ++beg ) {
		subs.push_back(*beg);
	}
	return subs;
}

void AcommsController::parseWaypoint( string msg ) {
	size_t index = msg.find(',');
	if ( index==string::npos || index==0 || index==msg.size()-1 ) {
		publishWarning("bad waypoint");
	} else {
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		vector<string> subs;
		boost::char_separator<char> sep(",");
		tokenizer tok(msg, sep);
		for ( tokenizer::iterator beg=tok.begin();
				beg!=tok.end(); ++beg ) {
			subs.push_back(*beg);
		}
		if ( subs.size()!=2 ) publishWarning("bad waypoint");
		else {
			m_target_x = atof(subs[0].c_str());
			m_target_y = atof(subs[1].c_str());
			stringstream ss;
			ss << m_target_x << "," << m_target_y;
			m_Comms.Notify("ACOMMS_CONTROLLER_WAYPOINT", ss.str() );
		}
	}
}

void AcommsController::publishWarning( string msg ) {
	m_Comms.Notify("ACOMMS_CONTROLLER_WARNING", msg);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AcommsController::OnConnectToServer() {
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	m_MissionReader.GetConfigurationParam("transmit_period", m_transmit_period);
	m_MissionReader.GetConfigurationParam("receive_delay", m_receive_delay);

	readMatrices();

	// state measurements
	m_Comms.Register("COMPASS_HEADING_UNFILITERED", 0);
	m_Comms.Register("COMPASS_HEADING_FILTERED", 0);
	m_Comms.Register("GPS_X", 0);
	m_Comms.Register("GPS_Y", 0);
	m_Comms.Register("GPS_SPEED", 0);

	// used to toggle controller on and off
	m_Comms.Register("MISSION_MODE", 0);
	// bridge shore driver status to SHORE_ACOMMS_STATUS
	
	// set waypoint target
	m_Comms.Register("ACOMMS_CONTROLLER_TARGET", 0);

	m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);
	m_Comms.Register("ACOMMS_BAD_FRAMES", 0);
	m_Comms.Register("ACOMMS_DRIVER_STATUS", 0);

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool AcommsController::Iterate() {
	if ( !active ) return true;

	if ( MOOSTime()-last_transmit_time > m_transmit_period ) {
		publishWarning("beginning next transmission");
		onTransmit();
		last_transmit_time = MOOSTime();
	}

	if ( MOOSTime()-last_receive_time > m_receive_delay-1 && !signal_sync_loss &&
			driver_status!="receiving" ) {
		publishWarning("expected receive start by now, will signal sync loss in one second");
		signal_sync_loss = true;
	}

	if ( MOOSTime()-last_receive_time > m_receive_delay && signal_sync_loss ) {
		publishWarning("signaling sync loss");
		signal_sync_loss = false;
		last_receive_time = MOOSTime();
		onLoss();
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool AcommsController::OnStartUp() {
	// happens before connection is open

	return (true);
}

