/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: acomms_timer.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "acomms_timer.h"
//---------------------------------------------------------
// Constructor

acomms_timer::acomms_timer()
{
	duty_cycle = 15;
	last_time = 0;
	rate = 0;
	pong_rate = 0;
	paused = true;
	driver_ready = false;
	counter = 0;
	data_out = "default";
	mode = "receive";
	size = 32;
}

//---------------------------------------------------------
// Destructor

acomms_timer::~acomms_timer()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool acomms_timer::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();

		if(key == "ACOMMS_TIMER_DUTY_CYCLE"){
			duty_cycle = msg.GetDouble();
			std::cout<<"Duty cycle updated: "<<duty_cycle<<std::endl;
		}

		else if(key == "ACOMMS_DRIVER_STATUS"){
			if(msg.GetString()=="ready"){
				driver_ready = true;
			}
			else{
				driver_ready = false;
			}
		}
		else if(key == "ACOMMS_TIMER_MODE"){
			mode = msg.GetString();
			std::cout<<"Mode updated: "<<mode<<std::endl;
		}

		else if(key == "ACOMMS_TIMER_PAUSED"){
			if(msg.GetString()=="true"){
				paused = true;
				std::cout<<"Mission paused."<<std::endl;
			}
			else{
				paused = false;
				std::cout<<"Mission unpaused."<<std::endl;
			}
		}

		else if(key=="ACOMMS_TRANSMIT_RATE"){
			rate = msg.GetDouble();
			std::cout<<"Rate updated: "<<rate<<std::endl;
		}

		else if(key=="ACOMMS_RECEIVED_SIMPLE"){
			lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO received(msg.GetString());
			if(mode=="pong"){
				if(rate != received.rate){
				m_Comms.Notify("ACOMMS_TRANSMIT_RATE",received.rate);
				}
			}
		}

		else if(key=="ACOMMS_RECEIVED_DATA"){
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",msg.GetString());
		}

		else if(key=="ACOMMS_TIMER_SIZE"){
			size = (int) msg.GetDouble();
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool acomms_timer::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));

	m_Comms.Register("ACOMMS_TIMER_PAUSED",0);
	m_Comms.Register("ACOMMS_TIMER_MODE",0);
	m_Comms.Register("ACOMMS_TIMER_DUTY_CYCLE",0);
	m_Comms.Register("ACOMMS_TIMER_SIZE",0);
	m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);

	m_Comms.Register("ACOMMS_DRIVER_STATUS",0);
	m_Comms.Register("ACOMMS_TRANSMIT_RATE",0);
	m_Comms.Register("ACOMMS_RECEIVED_DATA",0);

	return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool acomms_timer::Iterate()
{
	// happens AppTick times per second
	double time_passed = MOOSTime()-last_time;
	std::cout << "Time since last: " << time_passed << std::endl;

	if(time_passed >= duty_cycle){
		if(!paused && driver_ready){

			std::cout<<"Mode is "<<mode<<std::endl;

			if(mode=="transmit"){
				std::stringstream ss;
				ss << counter;
				data_out = ss.str()+"---"+getRandomString(getPacketSize(rate));
				std::cout<<"Transmitting: "<<data_out<<std::endl;;
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
				last_time = MOOSTime();
				counter++;
			}

			else if(mode=="switchtransmit"){

				if(rate==2){
					m_Comms.Notify("ACOMMS_TRANSMIT_RATE",0.0);
					std::stringstream ss;
					ss << counter;
					data_out = ss.str()+"---"+getRandomString(getPacketSize(rate));
					std::cout<<"Transmitting: "<<data_out;
					m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
				}

				else if(rate==0){
					m_Comms.Notify("ACOMMS_TRANSMIT_RATE",2);
					std::stringstream ss;
					ss << counter;
					data_out = ss.str()+"---"+getRandomString(getPacketSize(rate));
					std::cout<<"Transmitting: "<<data_out;
					m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
				}
				counter++;
				last_time = MOOSTime();
			}

			else if(mode == "sizetransmit"){
				std::stringstream ss;
				ss << counter;
				data_out = ss.str()+"---"+getRandomString(size);
				std::cout<<"Transmitting: "<<data_out;
				m_Comms.Notify("ACOMMS_TRANSMIT_DATA",data_out);
				counter++;
				last_time = MOOSTime();
			}

			else{
				paused = true;
				std::cout<<"Doing Nothing"<<std::endl;
				std::cout<<"Mode is: "<<mode<<std::endl;
			}
		}

		else{
			if(paused){std::cout<<"Mission paused."<<std::endl;}
			else if(!driver_ready){std::cout<<"Waiting for Modem Driver."<<std::endl;}
		}
	}

	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool acomms_timer::OnStartUp()
{
	// happens before connection is open

	return(true);
}

int acomms_timer::getPacketSize(int my_rate){
	int size;
	switch (my_rate) {
	case 0: size = 32; break;
	case 1: size = 192; break;
	case 2: size = 192; break;
	case 3: size = 512; break;
	case 4: size = 512; break;
	case 5: size = 2048; break;
	case 6: size = 192; break;

	default: size = 32; break;
	}

	return size;
}

std::string acomms_timer::getRandomString( int length ) {
	srand((unsigned) time(NULL));

	std::stringstream ss;
	const int passLen = length;
	for (int i = 0; i < passLen; i++) {
		char num = (char) ( rand() % 62 );
		if ( num < 10 )
			num += '0';
		else if ( num < 36 )
			num += 'A'-10;
		else
			num += 'a'-36;
		ss << num;
	}

	return ss.str();
}
