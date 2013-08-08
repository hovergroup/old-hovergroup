/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommStatusTransmit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AcommStatusTransmit.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AcommStatusTransmit::AcommStatusTransmit()
{
	m_period = 0;
	m_offset = 0;
	m_lastSentSlot = -1;
	enabled = false;
}

//---------------------------------------------------------
// Destructor

AcommStatusTransmit::~AcommStatusTransmit()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AcommStatusTransmit::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		if (key == "ASTATUS_TRANSMITS") {
			if (MOOSToUpper(msg.GetString())=="ON") {
				enabled = true;
			} else if (MOOSToUpper(msg.GetString())=="OFF") {
				enabled = false;
			}
		}else if (key == "NAV_X") {
            m_vx = msg.GetDouble();

        } else if (key == "NAV_Y") {
            m_vy = msg.GetDouble();

        }else if (key == "NAV_DEPTH") {
            m_vd = msg.GetDouble();

        } else if (key == "NAV_HEADING") {
            m_vb = msg.GetDouble();

        } else if (key == "NAV_SPEED") {
           m_vsp = msg.GetDouble();

        } else if (key == "ACOMMS_RECEIVED_DATA") {
        	//TODO design simple message class ?
        	//Receive Command from the shoreside, poor assumption,
        	// do some simple message "header" checking if possible in the future.
        	RemusAMessages::RemusCmdM cmd(msg.GetString());
        	m_Comms.Notify("DEPLOY", cmd.cmd); //# update this !
                if (cmd.cmd == "true")
        	    m_Comms.Notify("MOOS_MANUAL_OVERRIDE", "false"); //# update this !
                else
        	    m_Comms.Notify("MOOS_MANUAL_OVERRIDE", "true"); //# update this !

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

bool AcommStatusTransmit::OnConnectToServer()
{
	m_MissionReader.GetValue("Community", m_vname);
	MOOSToUpper(m_vname);

	bool ok1, ok2;
	ok1 = m_MissionReader.GetConfigurationParam("period", m_period);
	ok2 = m_MissionReader.GetConfigurationParam("offset", m_offset);

	if (!ok1 || !ok2) {
		std::cout << "Must define period and offset in configuration file."
				<< std::endl;
		exit(1);
	}

	m_Comms.Register("ASTATUS_TRANSMITS",0);
    m_Comms.Register("NAV_X", 0);
    m_Comms.Register("NAV_Y", 0);
    m_Comms.Register("NAV_DEPTH", 0);
    m_Comms.Register("NAV_HEADING", 0);
    m_Comms.Register("NAV_SPEED", 0);
    m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AcommStatusTransmit::Iterate()
{
	if (m_lastSentSlot == -1) {
		m_lastSentSlot = getNextSlot()-1;
	}

	if (getTime() > getSlotTime(m_lastSentSlot+1)) {
		if (enabled)
			post();

		m_lastSentSlot = getNextSlot()-1;
	}
	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AcommStatusTransmit::OnStartUp()
{
	return(true);
}

void AcommStatusTransmit::post() {
	std::stringstream ss;
        std::stringstream sa;
	sa << m_lastSentSlot++;
	m_Comms.Notify("ACOMMS_TCNT", sa.str());
	ss << getStatusString();
	m_Comms.Notify("ACOMMS_TRANSMIT_DATA", ss.str());
}

double AcommStatusTransmit::getTime() {
	return boost::posix_time::microsec_clock::local_time().
			time_of_day().total_milliseconds()/1000.0;
}

int AcommStatusTransmit::getNextSlot() {
	return ceil((getTime()-m_offset)/m_period);
}

double AcommStatusTransmit::getSlotTime(int slot) {
	return m_period*slot + m_offset;
}

std::string AcommStatusTransmit::getStatusString()
{
	RemusAMessages::RemusStatusM status;
	status.vname = m_vname;
	status.nav_x = m_vx;
	status.nav_y = m_vy;
	status.nav_d = m_vd;
	status.nav_b = m_vb;
        status.nav_s = m_vsp;
	return status.toString();

}

