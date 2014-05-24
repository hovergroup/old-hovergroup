/*
 * pAckedCommsShoreside
 *        File: pAckedCommsShoreside.cpp
 *  Created on: May 24, 2014
 *      Author: Josh Leighton
 */

#include <iterator>
#include "MBUtils.h"
#include "AckedCommsShoreside.h"
#include <boost/lexical_cast.hpp>
#include "ackedComms.pb.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AckedCommsShoreside::AckedCommsShoreside() {
    m_started = false;
    m_currentID = 0;
}

//---------------------------------------------------------
// Destructor

AckedCommsShoreside::~AckedCommsShoreside() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AckedCommsShoreside::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();

        // last instance of underscore separates variable from its destination
        int index = key.find_last_of('_');
        if (index != string::npos && index > 0 && index < key.length()-1) {
            // if underscore is found mid-key, separate var name and tail
            string tail = key.substr(index+1, key.length()-index-1);
            string var = key.substr(0, index);

            // we ignore broadcasts
            if (tail != "ALL") {
                // look for match within our registrations
                for (int i=0; i<m_vars.size(); i++) {
                    if (var == m_vars[i]) {
                        // construct a new transmission
                        AckedTransmission new_transmission;

                        new_transmission.var = var;
                        if (msg.IsDouble()) {
                            new_transmission.double_val = msg.GetDouble();
                            new_transmission.type = AckedTransmission::DOUBLE;
                        } else if (msg.IsBinary()) {
                            new_transmission.string_val = msg.GetString();
                            new_transmission.type = AckedTransmission::BINARY_STRING;
                        } else {
                            new_transmission.string_val = msg.GetString();
                            new_transmission.type = AckedTransmission::STRING;
                        }

                        new_transmission.delay = m_delays[var];
                        new_transmission.retries = m_repeats[var];
                        new_transmission.destination = tail;
                        new_transmission.id = m_currentID;
                        m_currentID++;

                        m_transmissions.push_back(new_transmission);

                        cout << "NEW TRANSMISSION" << endl;
                        cout << "    var = " << new_transmission.var << endl;
                        cout << "    type = ";
                        switch (new_transmission.type) {
                        case AckedTransmission::DOUBLE:
                            cout << "double" << endl;
                            break;
                        case AckedTransmission::BINARY_STRING:
                            cout << "binary string" << endl;
                            break;
                        case AckedTransmission::STRING:
                            cout << "string" << endl;
                            break;
                        }
                        cout << "    dest = " << new_transmission.destination << endl;
                        cout << "    id = " << new_transmission.id << endl << endl;

                        break;
                    }
                }
            }
        }
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AckedCommsShoreside::OnConnectToServer() {
    std::cout << "Connecting to server" << std::endl;
    if (!m_started) {
        cout << "First time connection." << endl;

        STRING_LIST Params;
        m_MissionReader.GetConfiguration(m_sAppName, Params);

        STRING_LIST::iterator p;
        for (p = Params.begin(); p != Params.end(); p++) {
            string sParam = *p;
            string sWhat = MOOSChomp(sParam, "=");

            // look for bridge configuration lines
            if (MOOSStrCmp(sWhat, "BRIDGE")) {
                string sNewVar = stripBlankEnds(sParam);
                string var;
                double repeat = 3, delay = 0.5;

                // parse the line
                while (sNewVar.find("=") != string::npos) {
                    string key = MOOSToUpper(MOOSChomp(sNewVar, "="));
                    string val = MOOSChomp(sNewVar, ",");
                    if (key == "VAR") {
                        var = val;
                    } else if (key == "REPEAT") {
                        try {
                            repeat = boost::lexical_cast<double>(val);
                        } catch (const boost::bad_lexical_cast &) {
                            cout << "Bad config." << endl;
                            exit(1);
                        }
                    } else if (key == "DELAY") {
                        try {
                            delay = boost::lexical_cast<double>(val);
                        } catch (const boost::bad_lexical_cast &) {
                            cout << "Bad config." << endl;
                            exit(1);
                        }
                    }
                }

                // check if we already have this variable
                if (find(m_vars.begin(), m_vars.end(), var) != m_vars.end()) {
                    cout << "Ignoring repeated bridge configuration" << endl;
                } else {
                    cout << "ADDING BRIDGE: " << endl;
                    cout << "    var = " << var << "*" << endl;
                    cout << "    repeat = " << repeat << endl;
                    cout << "    delay = " << delay << endl;

                    m_vars.push_back(var);
                    m_repeats[var] = repeat;
                    m_delays[var] = delay;

                    // wildcard registration
                    m_Comms.Register(var + "*", "*", 0);
                }
            }
        }
    } else {
        cout << "Not first time connection." << endl;
    }

    RegisterVariables();

    return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AckedCommsShoreside::Iterate() {
    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AckedCommsShoreside::OnStartUp() {
    return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void AckedCommsShoreside::RegisterVariables() {
// m_Comms.Register("FOOBAR", 0);
}

