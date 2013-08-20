/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AcommStatus13Bit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AcommStatus13Bit.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AcommStatus13Bit::AcommStatus13Bit()
{
    m_period = 0;
    m_offset = 0;
    m_lastSentSlot = -1;
    enabled = false;
}

//---------------------------------------------------------
// Destructor

AcommStatus13Bit::~AcommStatus13Bit()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AcommStatus13Bit::OnNewMail(MOOSMSG_LIST &NewMail)
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

          } else if (key == "ACOMMS_RECEIVED_DATA") {
              //TODO design simple message class ?
              //Receive Command from the shoreside, poor assumption,
              // do some simple message "header" checking if possible in the future.
              RemusAMessages::Remus13Bits m;
              m_Comms.Notify("REMUS_DEBUG", msg.GetString());
              std::string data = msg.GetString();
              unsigned char numCmd = data[0];

              m_Comms.Notify("MISSION_MODE", m.Num2Cmd((int)numCmd)); //# update this !
                  if (m.Num2Cmd((int)numCmd) == "INACTIVE")
                  m_Comms.Notify("MOOS_MANUAL_OVERRIDE", "true"); //# update this !
                  else
                  m_Comms.Notify("MOOS_MANUAL_OVERRIDE", "false"); //# update this !

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

bool AcommStatus13Bit::OnConnectToServer()
{
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
    m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);

    m_MissionReader.GetConfigurationParam("x_min", m_osx_minimum);
    m_MissionReader.GetConfigurationParam("x_max", m_osx_maximum);
    m_MissionReader.GetConfigurationParam("y_min", m_osy_minimum);
    m_MissionReader.GetConfigurationParam("y_max", m_osy_maximum);
	
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AcommStatus13Bit::Iterate()
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

bool AcommStatus13Bit::OnStartUp()
{

  return(true);
}

void AcommStatus13Bit::post() {
        std::stringstream sa;
    sa << m_lastSentSlot++;
    m_Comms.Notify("ACOMMS_TCNT", sa.str());
    getStatusString13Bits();
}

double AcommStatus13Bit::getTime() {
    return boost::posix_time::microsec_clock::local_time().
            time_of_day().total_milliseconds()/1000.0;
}

int AcommStatus13Bit::getNextSlot() {
    return ceil((getTime()-m_offset)/m_period);
}

double AcommStatus13Bit::getSlotTime(int slot) {
    return m_period*slot + m_offset;
}

void AcommStatus13Bit::getStatusString13Bits()
{
    RemusAMessages::Remus13Bits status;
    unsigned char transmit_x = status.LinearEncode(m_vx, m_osx_minimum, m_osx_maximum,5);
    unsigned char transmit_y = status.LinearEncode(m_vy, m_osy_minimum, m_osy_maximum,5);
    unsigned char range = 1;
        //  std::cout << "Follower range " << m_target_range <<
        //          " encoded as " << (int) range << std::endl;
        //    std::cout << "encoding: " << (int) transmit_x << " " << (int) transmit_y
        //            << " " << (int) range << std::endl;

            std::vector<unsigned char> data(2, 0);
            data[1] = (transmit_y<<3) + range;
            data[0] = transmit_x;

        //    std::cout << "sending: " << (int) data[0] << " " << (int) data[1]
        //            << std::endl;

            m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", &data[0], 2);

}


