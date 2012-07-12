/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: rateless_coding.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "rateless_coding.h"

//---------------------------------------------------------
// Constructor

rateless_coding::rateless_coding()
{
	last_time = 0;
	total_frames = 0;
	successful_frames = 0;
	counter = 0;
	duty_cycle=10;

	paused = true;
	driver_ready = false;
	waiting = false;
	received = false;

	rateless_index = 0;
	min_frames = 10;

	//pushback data here
}

//---------------------------------------------------------
// Destructor

rateless_coding::~rateless_coding()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool rateless_coding::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();

		if(key=="HEARD"){

			if(waiting){
				waiting = false;
				received = true;
			}

			lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info(msg.GetString());
			cout << "Got frames: " << receive_info.num_good_frames << endl;;
			successful_frames += receive_info.num_good_frames;
			cout << "Successful frames so far: " << successful_frames;
		}

		else if(key=="RATELESS_PAUSED"){
			if(msg.GetString() == "false"){
				paused = false;
			}
			else{paused = true;}
		}
		else if(key=="TRANSMITTER_STATUS"){
			if(msg.GetString()=="ready"){
				driver_ready = true;
			}
			else{driver_ready = false;}
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool rateless_coding::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_Comms.Register("RATELESS_PAUSED",0);
	m_Comms.Register("TRANSMITTER_STATUS",0);
	m_Comms.Register("HEARD",0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool rateless_coding::Iterate()
{
	// happens AppTick times per second

	if(successful_frames >= min_frames){
		stringstream ss;
		ss << "Success" << "," << total_frames << "," << successful_frames;
		ss.flush();
		m_Comms.Notify("RATELESS_STATS",ss.str());
		cout << ss.str() << endl;

		rateless_index = 0;
	}
	else{
		if(rateless_index == rateless_data.size()){
			stringstream ss;
			ss << "Failure" << "," << total_frames << "," << successful_frames;
			ss.flush();
			m_Comms.Notify("RATELESS_STATS",ss.str());
			cout << ss.str() << endl;

			rateless_index = 0;
		}
		else{
			cout << "Total transmissions: " << total_frames << endl;
			cout << "Waiting for more transmissions" << endl;
		}
	}

	double time_passed = MOOSTime()-last_time;
	std::cout << "Time since last: " << time_passed << std::endl;
	cout << "Data index is: " << rateless_index << endl;

	if(time_passed >= duty_cycle || received){
		if(!paused && driver_ready){

			if(received){
				received = false;
				cout << "Good Receipt." << endl;
			}

			m_Comms.Notify("TRANSMITTER_DATA",rateless_data[rateless_index]);
			cout << "Now Sending: " <<rateless_data[rateless_index] << endl;
			rateless_index++;
			total_frames++;
			waiting = true;
			last_time = MOOSTime();
		}

		else{
			if(paused){std::cout<<"Mission paused."<<std::endl;}
			else if(!driver_ready){std::cout<<"Waiting for Transmitter Driver."<<std::endl;}
		}
	}
	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool rateless_coding::OnStartUp()
{
	// happens before connection is open

	return(true);
}

