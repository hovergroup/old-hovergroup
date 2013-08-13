/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: KST.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "KST.h"

using namespace std;

//---------------------------------------------------------
// Constructor

KST::KST() {
}

//---------------------------------------------------------
// Destructor

KST::~KST() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool KST::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		if (msg.GetKey() == "NAV_X") {
			m_navy = msg.GetDouble();
		} else if (msg.GetKey() == "NAV_Y") {
			m_navx = msg.GetDouble();
		} else if (msg.GetKey() == "NAV_SPEED") {
			m_navspeed = msg.GetDouble();
		}

#if 0 // Keep these around just for template
		string key = msg.GetKey();
		string comm = msg.GetCommunity();
		double dval = msg.GetDouble();
		string sval = msg.GetString();
		string msrc = msg.GetSource();
		double mtime = msg.GetTime();
		bool mdbl = msg.IsDouble();
		bool mstr = msg.IsString();
#endif
	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool KST::OnConnectToServer() {
	RegisterVariables();
	m_startTime = MOOSTime();

	out.open("/home/josh/kst.csv");
	printHeader();

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool KST::Iterate() {
	printLine();
	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool KST::OnStartUp() {
	return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void KST::RegisterVariables() {
	m_Comms.Register("NAV_X", 0);
	m_Comms.Register("NAV_Y", 0);
	m_Comms.Register("NAV_SPEED", 0);
	// m_Comms.Register("FOOBAR", 0);
}

void KST::printHeader() {
	out << "time" << delim;
	out << "NAV_X" << delim;
	out << "NAV_Y" << delim;
	out << "NAV_SPEED" << std::endl;
}

void KST::printLine() {
	out << MOOSTime()-m_startTime << delim;
	out << m_navx << delim;
	out << m_navy << delim;
	out << m_navspeed << std::endl;
}
