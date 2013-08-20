/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommCmd13Bit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AcommCmd13Bit.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AcommCmd13Bit::AcommCmd13Bit()
{

}

//---------------------------------------------------------
// Destructor

AcommCmd13Bit::~AcommCmd13Bit()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AcommCmd13Bit::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        std::string key = msg.GetKey();

        if (key == "ACOMMS_RECEIVED_DATA") {
            //TODO design simple message class ?
            //Receive Status from the Remus, poor assumption,
            // do some simple message "header" checking if possible in the future.

            decodeStatusString13Bits( msg.GetString());


        }else if (key == "A_MISSIONMODE"){ // command
            RemusAMessages::Remus13Bits m;
            unsigned char cmd  = m.Cmd2Num(msg.GetString());
            std::vector<unsigned char> data (2, 0);
                data[1] = cmd;
                data[0] = cmd;
            m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY",  &data[0], 2);
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

bool AcommCmd13Bit::OnConnectToServer()
{
    // register for variables here
    // possibly look at the mission file?
    // m_MissionReader.GetConfigurationParam("Name", <string>);
    // m_Comms.Register("VARNAME", 0);

    m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);
    m_Comms.Register("A_MISSIONMODE", 0);

    m_MissionReader.GetConfigurationParam("x_min", m_osx_minimum);
    m_MissionReader.GetConfigurationParam("x_max", m_osx_maximum);
    m_MissionReader.GetConfigurationParam("y_min", m_osy_minimum);
    m_MissionReader.GetConfigurationParam("y_max", m_osy_maximum);

    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AcommCmd13Bit::Iterate()
{

    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AcommCmd13Bit::OnStartUp()
{

    return(true);
}

void AcommCmd13Bit::decodeStatusString13Bits(const std::string data)
{
    if ( data.size() != 2 ) {
        std::stringstream ss;
        ss << "Wrong data length: " << data.size();
        m_Comms.Notify("REMUS_DEBUG", ss.str());
    }

    RemusAMessages::Remus13Bits msg;

    double received_x = msg.LinearDecode( data[0], m_osx_minimum, m_osx_maximum, 5);
    double received_y = msg.LinearDecode( data[1]>>3, m_osy_minimum, m_osy_maximum, 5);

    m_Comms.Notify("REMUS_X",received_x);
    m_Comms.Notify("REMUS_Y",received_y);
}



