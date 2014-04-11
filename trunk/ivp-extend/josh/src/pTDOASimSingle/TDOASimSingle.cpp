/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOASimSingle.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "TDOASimSingle.h"
#include "HoverAcomms.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TDOASimSingle::TDOASimSingle()
{
    m_lastUpdate = 0;
    m_state = LEADER_SLOT;
    m_period = 10;
    m_paused = true;
    srand (time(NULL));
}

//---------------------------------------------------------
// Destructor

TDOASimSingle::~TDOASimSingle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TDOASimSingle::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();

        if (key == "TDOA_COMMAND") {
            string sval = MOOSToUpper(msg.GetString());
            if (sval == "RUN") {
                m_paused = false;
                m_outgoingUpdate.mutable_data()->Clear();
                m_state = LEADER_SLOT;
            } else if (sval == "PAUSE") {
                m_paused = true;
                m_state = PAUSED;
            }
        } else if (key == "F1X") {
            m_f1x = msg.GetDouble();
        } else if (key == "F1Y") {
            m_f1y = msg.GetDouble();
        } else if (key == "F2X") {
            m_f2x = msg.GetDouble();
        } else if (key == "F2Y") {
            m_f2y = msg.GetDouble();
        } else if (key == "F3X") {
            m_f3x = msg.GetDouble();
        } else if (key == "F3Y") {
            m_f3y = msg.GetDouble();
        } else if (key == "TARGETX") {
            m_targetx = msg.GetDouble();
        } else if (key == "TARGETY") {
            m_targety = msg.GetDouble();
        }
    }

    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TDOASimSingle::OnConnectToServer()
{
    m_MissionReader.GetConfigurationParam("f1x", m_f1x);
    m_MissionReader.GetConfigurationParam("f1y", m_f1y);
    m_MissionReader.GetConfigurationParam("f2x", m_f2x);
    m_MissionReader.GetConfigurationParam("f2y", m_f2y);
    m_MissionReader.GetConfigurationParam("f3x", m_f3x);
    m_MissionReader.GetConfigurationParam("f3y", m_f3y);
    m_MissionReader.GetConfigurationParam("targetx", m_targetx);
    m_MissionReader.GetConfigurationParam("targety", m_targety);
    m_MissionReader.GetConfigurationParam("update_period", m_period);

    cout << "Period set to " << m_period << endl;

    m_outgoingUpdate.set_local_id(1);
    m_outgoingUpdate.set_cycle_state(TDOAUpdate_StateEnum_PAUSED);

    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOASimSingle::Iterate()
{
    if (m_paused)
        return true;

    if (MOOSTime() - m_lastUpdate > m_period) {
        switch (m_state) {
        case LEADER_SLOT:
            m_outgoingUpdate.mutable_data()->Clear();
            calculateRanges();
            m_randomOffset = ((double) rand())/ RAND_MAX;
            cout << "The random offset for this cycle is "
                    << m_randomOffset << " seconds." << endl;
            addData(m_f1x, m_f1y, 1, m_range1 / 1492 + m_randomOffset);
            postOutput();
            m_state = F1_SLOT;
            break;
        case F1_SLOT:
            postOutput();
            m_state = F2_SLOT;
            break;
        case F2_SLOT:
            addData(m_f2x, m_f2y, 2, m_range2 / 1492 + m_randomOffset);
            postOutput();
            m_state = F3_SLOT;
            break;
        case F3_SLOT:
            addData(m_f3x, m_f3y, 3, m_range3 / 1492 + m_randomOffset);
            postOutput();
            m_state = LEADER_SLOT;
            break;
        }
        m_lastUpdate = MOOSTime();
    }

    return(true);
}

void TDOASimSingle::addData(double x, double y, int id, double time) {
    TDOAData* dat = m_outgoingUpdate.add_data();
    dat->set_x(x);
    dat->set_y(y);
    dat->set_id(id);
    dat->set_toa(time);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TDOASimSingle::OnStartUp()
{
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void TDOASimSingle::RegisterVariables()
{
    m_Comms.Register("F1X", 0);
    m_Comms.Register("F1Y", 0);
    m_Comms.Register("F2X", 0);
    m_Comms.Register("F2Y", 0);
    m_Comms.Register("F3X", 0);
    m_Comms.Register("F3Y", 0);
    m_Comms.Register("TARGETX", 0);
    m_Comms.Register("TARGETY", 0);

    m_Comms.Register("TDOA_COMMAND", 0);
    // m_Comms.Register("FOOBAR", 0);
}

void TDOASimSingle::calculateRanges() {
    m_range1 = sqrt(pow(m_f1x-m_targetx, 2.0) + pow(m_f1y-m_targety, 2.0));
    m_range2 = sqrt(pow(m_f2x-m_targetx, 2.0) + pow(m_f2y-m_targety, 2.0));
    m_range3 = sqrt(pow(m_f3x-m_targetx, 2.0) + pow(m_f3y-m_targety, 2.0));
}

void TDOASimSingle::postOutput() {
    m_outgoingUpdate.set_cycle_state((TDOAUpdate_StateEnum) m_state);
    string out = m_outgoingUpdate.SerializeAsString();
    m_Comms.Notify("TDOA_PROTOBUF", (void*) out.data(), out.size());
    out = m_outgoingUpdate.DebugString();
    while (out.find("\n") != std::string::npos) {
        out.replace(out.find("\n"), 1, "<|>");
    }
    m_Comms.Notify("TDOA_PROTOBUF_DEBUG", out);
    cout << out << endl << endl;
}

