/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommCmdTransmit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AcommCmdTransmit.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AcommCmdTransmit::AcommCmdTransmit()
{
}

AcommCmdTransmit::~AcommCmdTransmit()
{
}
//---------------------------------------------------------
// Procedure: OnNewMail

bool AcommCmdTransmit::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();

		if (key == "ACOMMS_RECEIVED_DATA") {
			//TODO design simple message class ?
			//Receive Status from the Remus, poor assumption,
			// do some simple message "header" checking if possible in the future.
			RemusAMessages::RemusStatusM status(msg.GetString());
			m_Comms.Notify("REMUS_X",status.nav_x);
			m_Comms.Notify("REMUS_Y",status.nav_y);
			m_Comms.Notify("REMUS_DEPTH",status.nav_d);
			m_Comms.Notify("REMUS_YAW",status.nav_b);

		}else if (key == "A_DEPLOY"){ // command
			RemusAMessages::RemusCmdM cmd(msg.GetString());
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA", cmd.toString());
		}

#if 0 // Keep these around just for template
		string key   = msg.GetKey();
		string comm  = msg.GetCommunity();
		double dval  = msg.GetDouble();
		string sval  = msg.GetString();
		string msrc  = msg.GetSource();
		double mtime = msg.GetTime();
		bool   mdbl  = msg.IsDouble();
		bool   mstr  = msg.IsString();
#endif
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AcommCmdTransmit::OnConnectToServer()
{

	m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AcommCmdTransmit::Iterate()
{
	return(true);
}


