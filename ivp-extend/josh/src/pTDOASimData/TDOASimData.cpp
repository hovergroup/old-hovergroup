/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: TDOASimData.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "TDOASimData.h"
#include "HoverAcomms.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TDOASimData::TDOASimData() : input("amalgamated.csv")
{
    m_lastUpdate = 0;
    m_state = LEADER_SLOT;
    m_period = 10;
    m_paused = true;

    char buf[1024];
    input.getline(buf, 1024);
    cout << buf << endl;
}

//---------------------------------------------------------
// Destructor

TDOASimData::~TDOASimData()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TDOASimData::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
    }

    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TDOASimData::OnConnectToServer()
{
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TDOASimData::Iterate()
{
    if (cin.get()) {
        char buf[1024];
        input.getline(buf, 1024);
        string buff(buf);
        cout << buff << endl;
        double time=-1, targx=-1, targy=-1;
        vector<double> x(3,-1), y(3,-1), toa(3,-1);
        vector<int>id (3,-1);
        try {
            time = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            targx = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            targy = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            id[0] = boost::lexical_cast<int>(MOOSChomp(buff, ","));
            x[0] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            y[0] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            toa[0] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            id[1] = boost::lexical_cast<int>(MOOSChomp(buff, ","));
            x[1] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            y[1] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            toa[1] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            id[2] = boost::lexical_cast<int>(MOOSChomp(buff, ","));
            x[2] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            y[2] = boost::lexical_cast<double>(MOOSChomp(buff, ","));
            toa[2] = boost::lexical_cast<double>(buff);
        } catch (exception e) {

        }
//        sscanf(buf, "%f,%f,%f,%d,%f,%f,%f,%d,%f,%f,%f,%d,%f,%f,%f",
//                &time,
//                &targx,
//                &targy,
//                &id[0],
//                &x[0],
//                &y[0],
//                &toa[0],
//                &id[1],
//                &x[1],
//                &y[1],
//                &toa[1],
//                &id[2],
//                &x[2],
//                &y[2],
//                &toa[2] );
        m_outgoingUpdate.Clear();
        for (int i=0; i<3; i++) {
            if (id[i] != -1) {
                TDOAData * dat = m_outgoingUpdate.add_data();
                dat->set_id(id[i]);
                dat->set_x(x[i]);
                dat->set_y(y[i]);
                dat->set_toa(toa[i]);
                m_outgoingUpdate.set_local_id(1);
            }
        }
        switch (m_outgoingUpdate.data_size()) {
        case 1:
            m_outgoingUpdate.set_cycle_state(TDOAUpdate_StateEnum_LEADER_SLOT_COMPLETE);
            break;
        case 2:
            m_outgoingUpdate.set_cycle_state(TDOAUpdate_StateEnum_F2_SLOT_COMPLETE);
            break;
        case 3:
            m_outgoingUpdate.set_cycle_state(TDOAUpdate_StateEnum_F3_SLOT_COMPLETE);
            break;
        }

        cout << "Target: " << targx << "  " << targy << endl;
        cout << m_outgoingUpdate.DebugString() << endl << endl;


        string out = m_outgoingUpdate.SerializeAsString();
        m_Comms.Notify("TDOA_PROTOBUF", (void*) out.data(), out.size());
        stringstream marker;
        marker << "label=target,x=" << targx << ",y=" << targy;
        m_Comms.Notify("VIEW_MARKER", marker.str());
        for (int i=0; i<3; i++) {
            if (id[i] != -1) {
                marker.str("");
                marker << "label=f" << id[i] << ",x=" << x[i] << ",y=" << y[i];
                m_Comms.Notify("VIEW_MARKER", marker.str());
            }
        }

    }

    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TDOASimData::OnStartUp()
{
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void TDOASimData::RegisterVariables()
{
}

