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

	total_successes =0;
	total_failures =0;

	paused = true;
	driver_ready = false;
	waiting = false;
	received = false;
	started_transmitting = false;

	rateless_index = 0;
	min_frames = 10;

	loss_total = 0;
	loss_success = 0;

	print_out = false;

	//pushback data here
	rateless_data.push_back("LB@@9:EQ27JHG;QFL8ML:NIN56FDH6M3");
	rateless_data.push_back("<Q=A<M9C?@DMOQJM8BPF322529Q5A834");
	rateless_data.push_back("M4?2=9MD:L6N2=49M4QDKIO=L3QH4JJ5");
	rateless_data.push_back("P98H54P=?Q>9A4ECP29M:LLCFFCLBE66");
	rateless_data.push_back("B3:9>J<7O2I@N5M9@:5A9ONHEB@?MB57");
	rateless_data.push_back("7A>7AOAAL99MN3=D634<=H78@COIBPG8");
	rateless_data.push_back("EL23N2@?HGO:J9;G3I8F:<C@NE>I5MM9");
	rateless_data.push_back("4L7FEKO<K?P?<=8B95L3NKILAAF@L8J:");
	rateless_data.push_back("I:33H:;LNMM7;@6KE<PB@?DJMA<M:KI;");
	rateless_data.push_back(":94M638JJ=4JQ;HG5N=A74PP:I;P67A<");
	rateless_data.push_back("<HJI84>5E6:=4:GCJ=<GE3LO7N33KAK=");
	rateless_data.push_back("8;4HH8ALJ7>J;P;L=E92Q62KB@2;OE@>");
	rateless_data.push_back("B8@=3K6DG2D9KBQJ4:N7I?5FI;H<<=3?");
	rateless_data.push_back("ID6;:M?8=D4JCI9JJ3PL7=JFG:FQ6EP@");
	rateless_data.push_back("IC>HQ95@85A>M<N6;Q:<<BEOA5NBD8?A");
	rateless_data.push_back("8GDL8B<5;4QGJO9C:CM?P4DKLKB@H;3B");
	rateless_data.push_back("JHOH9DBPM543@A9EGGHCCKIOLL:?7HOC");
	rateless_data.push_back("8BOOLMHADB7<9GNI52Q@4=><CDML@Q5D");
	rateless_data.push_back("P;>H2:E7DEOJL=<89MB@E5IDLQ>2J=3E");
	rateless_data.push_back("46D5F3L3E:B66CGC7<687EHLM7IPIK>F");
	rateless_data.push_back(">7<6A759;FFG77;A4C6N@K6PFD=I75NG");
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

			if(started_transmitting){
				if(waiting){
					waiting = false;
					received = true;
				}

				cout << endl;
				lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info(msg.GetString());
				cout << "Got frames: " << receive_info.num_good_frames << endl;;
				successful_frames += receive_info.num_good_frames;
				loss_success += receive_info.num_good_frames;
				cout << "Successful frames so far: " << successful_frames << endl;
			}
		}

		else if(key=="RATELESS_PAUSED"){
			if(msg.GetString() == "false"){
				paused = false;
			}
			else{paused = true;}
		}
		else if(key=="TRANSMITTER_STATUS"){
			//cout << msg.GetString();
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

	m_Comms.Notify("RATELESS_STATS","RESETTING,0,0");

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
		total_successes++;
		cout << "Successes so far: "<< total_successes << endl;
		successful_frames = 0;
		total_frames = 0;
	}
	else{
		if(rateless_index == rateless_data.size()){
			stringstream ss;
			ss << "Failure" << "," << total_frames << "," << successful_frames;
			ss.flush();
			m_Comms.Notify("RATELESS_STATS",ss.str());
			cout << ss.str() << endl;

			rateless_index = 0;
			total_failures++;
			cout << "Failures so far: " << total_failures << endl;
			successful_frames = 0;
			total_frames = 0;
		}
		else{
			print_out = true;
		}
	}

	double time_passed = MOOSTime()-last_time;

	if(time_passed >= duty_cycle || received){
		std::cout << "Time since last: " << time_passed << std::endl;
		cout << "Data index is: " << rateless_index << endl;

		if(print_out){
			cout << "Total transmissions: " << total_frames << endl;
			cout << "Waiting for more transmissions" << endl;
			print_out = false;
		}

		if(!paused && driver_ready){

			if(received){
				received = false;
				cout << "Good Receipt." << endl;
			}
			else if(!received && waiting){
				cout << endl;
				cout << "Missed Sync" << endl;
			}

			double loss_rate = (loss_total - loss_success) / loss_total;
			cout << "Loss Rate is: " << loss_rate << endl;
			m_Comms.Notify("TRANSMITTER_DATA",rateless_data[rateless_index]);
			started_transmitting = true;
			cout << "Now Sending: " <<rateless_data[rateless_index] << endl;
			rateless_index++;
			total_frames++;
			loss_total++;
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

