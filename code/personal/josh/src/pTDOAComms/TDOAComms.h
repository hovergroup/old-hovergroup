/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOAComms.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TDOAComms_HEADER
#define TDOAComms_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "tdoa.pb.h"
#include "JoshUtils.h"
#include "goby/acomms/dccl.h"

enum StateEnum {
	LEADER_SLOT = 0,
	F1_SLOT,
	F2_SLOT,
	F3_SLOT,
	PAUSED
};

class TDOAComms: public CMOOSApp {
public:
	TDOAComms();
	~TDOAComms();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	// Configuration variables
	int m_id, m_targetID;
	double m_targetOffset, m_localOffset;
	std::vector<double> m_offsets;

private:
	// State variables
	bool m_paused;
	int m_state, m_lastOutputState;
	double m_lastStateAdvanceTime;

	double m_navX, m_navY;

private:
	// other stuff
	TDOAUpdate m_outgoingUpdate;
	void resetOutput();
	JoshUtil::SlotFunctions m_slotFunctions;

	bool testAdvanceSlot(double offset);
	void advanceSlot();

	void acommsTransmit();
	void acommsReceive(std::string msg);

	void postOutput();

	goby::acomms::DCCLCodec* m_codec;
};

#endif 
