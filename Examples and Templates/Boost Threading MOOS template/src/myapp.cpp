/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: myapp.cpp                                          */
/*    DATE: June 26th, 2008                                      */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iterator>
#include "myapp.h"
#include "lib_mbutil/MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

myapp::myapp()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool myapp::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key  = msg.GetKey();
		
		if (key == "LISTEN_VARIABLE") {
			cout << msg.GetString() << endl;
			// for doubles, msg.GetDouble()
		}
	}

	return(true);
}
	
//---------------------------------------------------------
// Procedure: OnConnectToServer

bool myapp::OnConnectToServer()
{
	// I prefer to read my config file here, so I can be sure I finish reading it before doing anything else
	STRING_LIST sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	m_MissionReader.GetConfiguration(GetAppName(), sParams);
	
	STRING_LIST::iterator p;
	for (p=sParams.begin(); p!=sParams.end(); p++) {
		string sLine = *p;
		string sVarName = MOOSChomp(sLine, "=");
		sLine = stripBlankEnds(sLine);
		
		// this is the variable name we found
		cout << sVarName << endl;
		
		// match names to what you expect, and parse the values
		if (MOOSStrCmp(sVarName, "BAUD_RATE")) {
			if(!strContains(sLine, " "))
				int baudRate = boost::lexical_cast<int>(stripBlankEnds(sLine));
		}
	}
	
	RegisterVariables();
	
	mythread = boost::thread(boost::bind(&myapp::thread_loop, this));
	
	return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void myapp::RegisterVariables()
{
	m_Comms.Register("LISTEN_VARAIBLE", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool myapp::Iterate()
{
	m_Comms.Notify("VARIABLE_NAME", "I did something.");
	return(true);
}



//---------------------------------------------------------
// Procedure: OnStartUp()
//      Note: happens before connection is open

bool myapp::OnStartUp()
{
	// I prefer to do nothing here
	return(true);
}

void myapp::thread_loop() {
	// do stuff
}
