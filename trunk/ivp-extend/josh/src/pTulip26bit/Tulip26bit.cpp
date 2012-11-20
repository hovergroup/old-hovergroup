/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Tulip26bit.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "Tulip26bit.h"
#include "goby/acomms/connect.h"

//---------------------------------------------------------
// Constructor

Tulip26bit::Tulip26bit() {
    m_lastRangeRequestTime = 0;
    m_range_source = "sim";
    m_target_acomms_id = -1;

    m_gotSource = false;
    m_gotRange = false;
    m_gotFrames = false;
    m_gotData = false;
}

//---------------------------------------------------------
// Destructor

Tulip26bit::~Tulip26bit() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Tulip26bit::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        std::string key = msg.GetKey();

        if (key == "ACOMMS_DRIVER_STATUS") {
            std::string status = msg.GetString();
            if (m_AcommsStatus != "receiving" && status == "receiving")
                m_AcommsTimer.signalStartOfModemReceiving(msg.GetTime());
            m_AcommsStatus = status;

        } else if (key == "ACOMMS_RECEIVED_DATA") {
            m_ReceivedData = msg.GetString();
            m_ReceivedDataTime = msg.GetTime();
            m_gotData = true;

        } else if (key == "ACOMMS_BAD_FRAMES") {
            m_FrameStatus = msg.GetString();
            m_BadFramesTime = msg.GetTime();
            m_gotFrames = true;

        } else if (key == "ACOMMS_SOURCE_ID") {
            m_sourceID = msg.GetDouble();
            m_gotSource = true;

        } else if (key == "ACOMMS_ONE_WAY_TRAVEL_TIME") {
            m_acommsRange = msg.GetDouble() * 1450.0;
            m_gotRange = true;

        } else if (key == "GPS_TIME_SECONDS") {
            m_AcommsTimer.processGpsTimeSeconds(msg.GetDouble(), msg.GetTime());

        } else if (key == "NAV_X") {
            m_osx = msg.GetDouble();

        } else if (key == "NAV_Y") {
            m_osy = msg.GetDouble();

        } else if ( key == "TARGET_RANGE_RETURN" ) {
            RangeSensorTypes::RangeReply rr(msg.GetString());
            m_target_range = rr.range;

        } else if ( key == "FOLLOWER_WAYPOINT" ) {
            std::string sline = msg.GetString();
            std::string sTmp = MOOSChomp(sline,",");
            m_set_x = atof(sTmp.c_str());
            m_set_y = atof(sline.c_str());

        } else if ( key == "LEADER_WAYPOINT" ) {
            std::stringstream ss;
            ss << "points=" << m_osx << "," << m_osy << ":" << msg.GetString();
            m_Comms.Notify("TULIP_WAYPOINT_UPDATES", ss.str());
            m_Comms.Notify("TULIP_STATION", "false");

        }
    }

    if ( m_gotSource && m_gotRange && m_gotFrames && m_gotData ) {
        if ( m_sourceID == m_target_acomms_id ) {
            m_target_range = m_acommsRange;
            std::stringstream ss;
            ss << "range to target: " << m_target_range;
            handleUpdate( ss.str() );
            m_AcommsTimer.signalBreakFromReceiving();
        } else {
            if (m_FrameStatus == "" )
                m_AcommsTimer.signalGoodReception(m_ReceivedData);
            else
                m_AcommsTimer.signalBadReception();
        }

        m_gotSource = false;
        m_gotRange = false;
        m_gotFrames = false;
        m_gotData = false;
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Tulip26bit::OnConnectToServer() {
    std::string follower_range_divs;
    m_MissionReader.GetConfigurationParam("follower_range_divs", follower_range_divs);
    while ( follower_range_divs.size() > 0 ) {
        std::string sTmp = MOOSChomp(follower_range_divs,",");
        m_follower_range_divs.push_back(atof(sTmp.c_str()));
    }
    std::stringstream ss;
    for ( int i=0; i<m_follower_range_divs.size(); i++ ) {
        ss << m_follower_range_divs[i] << ",";
    }
    m_Comms.Notify("TULIP_RANGE_DIVS", ss.str());

    m_MissionReader.GetConfigurationParam("x_min", m_osx_minimum);
    m_MissionReader.GetConfigurationParam("x_max", m_osx_maximum);
    m_MissionReader.GetConfigurationParam("y_min", m_osy_minimum);
    m_MissionReader.GetConfigurationParam("y_max", m_osy_maximum);

    double receive_period, receive_offset;
    m_MissionReader.GetConfigurationParam("receive_period", receive_period);
    m_MissionReader.GetConfigurationParam("receive_offset", receive_offset);
    m_AcommsTimer.setReceiveTiming(receive_period, receive_offset);

    double transmit_period, transmit_offset;
    m_MissionReader.GetConfigurationParam("transmit_period", transmit_period);
    m_MissionReader.GetConfigurationParam("transmit_offset", transmit_offset);
    m_AcommsTimer.setTransmitTiming(transmit_period, transmit_offset);

    m_MissionReader.GetValue("Community", m_name);
    MOOSToUpper(m_name);

    double receive_extension, max_receive_error;
    m_MissionReader.GetConfigurationParam("receive_extension",
            receive_extension);
    m_MissionReader.GetConfigurationParam("max_receive_error",
    		max_receive_error);
    m_AcommsTimer.setReceivingExtension(receive_extension);
    m_AcommsTimer.setMaxReceivingError(max_receive_error);


    std::string vehicle_mode;
    m_MissionReader.GetConfigurationParam("vehicle_mode", vehicle_mode);

    std::string range_source = "sim";
    m_MissionReader.GetConfigurationParam("range_source", range_source);
    m_MissionReader.GetConfigurationParam("target_id", m_target_acomms_id);

    if (vehicle_mode == "leader") {
        goby::acomms::connect(&m_AcommsTimer.signal_transmit,
                boost::bind(&Tulip26bit::onTransmit_leader, this));
        goby::acomms::connect(&m_AcommsTimer.signal_no_receipt,
                boost::bind(&Tulip26bit::onBadReceive_leader, this));
        goby::acomms::connect(&m_AcommsTimer.signal_receipt,
                boost::bind(&Tulip26bit::onGoodReceive_leader, this, _1));

        m_Comms.Register("FOLLOWER_WAYPOINT",0);
        m_Comms.Register("LEADER_WAYPOINT",0);
    } else if (vehicle_mode == "follower") {
        goby::acomms::connect(&m_AcommsTimer.signal_transmit,
                boost::bind(&Tulip26bit::onTransmit_follower, this));
        goby::acomms::connect(&m_AcommsTimer.signal_no_receipt,
                boost::bind(&Tulip26bit::onBadReceive_follower, this));
        goby::acomms::connect(&m_AcommsTimer.signal_receipt,
                boost::bind(&Tulip26bit::onGoodReceive_follower, this, _1));

    } else {
        std::cout << "Exiting on invalid vehicle mode: " << vehicle_mode
                << std::endl;
        exit(0);
    }

    goby::acomms::connect(&m_AcommsTimer.signal_debug,
            boost::bind(&Tulip26bit::handleDebug, this, _1));
    goby::acomms::connect(&m_AcommsTimer.signal_updates,
            boost::bind(&Tulip26bit::handleUpdate, this, _1));

    if ( range_source == "sim" ) {
        m_range_source = "sim";
        m_Comms.Register("TARGET_RANGE_RETURN", 0);
    } else if ( range_source == "acomms" ) {
        m_range_source = "acomms";
        m_Comms.Register("ACOMMS_ONE_WAY_TRAVEL_TIME", 0);
        m_Comms.Register("ACOMMS_SOURCE_ID", 0);
    } else {
        std::cout << "invalid range source: " << range_source << std::endl;
        return false;
    }

    m_Comms.Register("ACOMMS_RECEIVED_DATA", 0);
    m_Comms.Register("ACOMMS_DRIVER_STATUS", 0);
    m_Comms.Register("ACOMMS_BAD_FRAMES", 0);
    m_Comms.Register("GPS_TIME_SECONDS", 0);

    m_Comms.Register("NAV_X", 0);
    m_Comms.Register("NAV_Y", 0);

    return (true);
}

void Tulip26bit::handleDebug(const std::string msg) {
    m_Comms.Notify("TULIP_DEBUG", msg);
}

void Tulip26bit::handleUpdate(const std::string msg) {
    m_Comms.Notify("TULIP_UPDATES", msg);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Tulip26bit::Iterate() {
    m_AcommsTimer.doWork(MOOSTime());

    if (MOOSTime() - m_lastRangeRequestTime > .5 && m_range_source == "sim") {
        RangeSensorTypes::RangeRequest request;
        request.vname = m_name;
        request.nav_x = m_osx;
        request.nav_y = m_osy;
        m_Comms.Notify("TARGET_RANGE_REQUEST", request.toString());
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Tulip26bit::OnStartUp() {
    // happens before connection is open

    return (true);
}

unsigned char Tulip26bit::LinearEncode(double val, double min, double max,
        int bits) {
    unsigned char transmit_val;
    if (val <= min)
        return 0;
    else if (val >= max)
        return pow(2, bits) - 1;
    else {
        double ratio = (val - min) / (max - min);
        double scaled = ratio * (pow(2, bits) - 1);
        return floor(scaled + .5);
    }
}

double Tulip26bit::LinearDecode(unsigned char val, double min, double max,
        int bits) {
    double ratio = val / (pow(2.0, bits) - 1.0);
    return min + ratio * (max - min);
}

unsigned char Tulip26bit::FlexibleEncode(double val,
        std::vector<double> & range_divs, int bits) {

    if (range_divs.size() != pow(2, bits) - 1) {
        std::stringstream ss;
        ss << "Specified number of bits (" << bits;
        ss << ") does not match vector size (" << range_divs.size();
        ss << ")";
        handleDebug(ss.str());
        return 0x00;
    }

    unsigned char transmit_val;

    for (int i = 0; i < range_divs.size(); i++) {
        if ( val < range_divs[i] )
            return i;
    }
    return range_divs.size();
}



