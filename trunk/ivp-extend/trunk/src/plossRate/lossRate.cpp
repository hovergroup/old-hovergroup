/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: lossRate.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "lossRate.h"

//---------------------------------------------------------
// Constructor

lossRate::lossRate()
{
	timeout = 600;
	listening = false;
	clear_old = false;
}

//---------------------------------------------------------
// Destructor

lossRate::~lossRate()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool lossRate::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      std::string key = msg.GetKey();

      if(key=="LOSS_RATE_RESET"){
    	  all_frames.clear();
    	  bad_frames.clear();
    	  good_frames.clear();

    	  sync.clear();
    	  loss.clear();
    	  success.clear();
    	  expected.clear();
    	  total_expected.clear();
    	  std::cout<<"Resetting: "<<sync.size()<<std::endl;
      }

      else if(key=="NODE_REPORT"){
    	  std::string name = msg.GetCommunity();
    	  if(vehicles.count(name)==0){
    		  vehicles.insert(name);
    		  std::cout<<"New Vehicle Found: "<<name<<std::endl;
    	  }
      }

      else if(key=="ACOMMS_RECEIVED_SIMPLE"){
    	  lib_acomms_messages::SIMPLIFIED_RECEIVE_INFO receive_info(msg.GetString());

		  all_frames[receive_info.vehicle_name] = receive_info.num_frames;
		  good_frames[receive_info.vehicle_name] = receive_info.num_good_frames;
		  bad_frames[receive_info.vehicle_name] = receive_info.num_bad_frames;
      }

      else if(key=="ACOMMS_TRANSMIT_SIMPLE"){
    	  lib_acomms_messages::SIMPLIFIED_TRANSMIT_INFO transmit_info(msg.GetString());
    	  last_time = MOOSTime();
    	  transmitter = transmit_info.vehicle_name;

    	  if(listening){

    		  std::set<std::string>::iterator it;
			  for( it = vehicles.begin(); it != vehicles.end(); it++ ) {
				std::string my_key = transmitter+*it;
				expected[my_key] = transmit_frames;
				total_expected[my_key] += transmit_frames;
			  }

    		  transmit_frames = transmit_info.num_frames;
    		  clear_old = true;
    	  }
    	  else{
    		  transmit_frames = transmit_info.num_frames;
    		  listening = true;
    		  clear_old = false;}
      }
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool lossRate::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	m_Comms.Register("NODE_REPORT",0);
	m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
	m_Comms.Register("ACOMMS_TRANSMIT_SIMPLE",0);
	m_Comms.Register("LOSS_RATE_RESET",0);

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool lossRate::Iterate()
{
   // happens AppTick times per second
	if(MOOSTime()-last_time>timeout||clear_old)
	{
		  std::set<std::string>::iterator it;
		  for( it = vehicles.begin(); it != vehicles.end(); it++ ) {

			  std::string my_key = transmitter+*it;

			  if(all_frames.count(*it)==0){
				sync[my_key] += expected[my_key];
			  }

			  else{
				  	loss[my_key] += bad_frames[*it];
				  	success[my_key] += good_frames[*it];
			  }

			  std::cout<< my_key<<std::endl;
			  std::cout<<" Expected: "<<expected[my_key]<< " frames"<<std::endl;
			  std::cout<<" Got: "<<all_frames[*it]<< " frames"<<std::endl;
			  std::cout<<" Good: "<<good_frames[*it]<< " frames"<<std::endl;
			  std::cout<<" Bad: "<<bad_frames[*it]<< " frames"<<std::endl<<std::endl;

			  double sync_loss = sync[my_key]/total_expected[my_key];
			  double crc_loss = loss[my_key]/total_expected[my_key];
			  double success_rate = success[my_key]/total_expected[my_key];

			  std::cout<< "Sync Loss Rate: "<<sync_loss<<std::endl;
			  std::cout<< "Bad CRC Loss Rate: " << crc_loss <<std::endl;
			  std::cout<< "Success Rate: " << success_rate <<std::endl;
			  std::cout<< "Total Sent: "<<total_expected[my_key]<<std::endl<<std::endl;

//			  lib_acomms_messages::LOSS_RATE_INFO all_losses(transmitter,*it,sync_loss, crc_loss, success_rate, total_expected[my_key]);
//			  m_Comms.Notify("ACOMMS_LOSS_RATE_INFO",all_losses.serializeToString());
		  }

		all_frames.clear();
		bad_frames.clear();
		good_frames.clear();
		clear_old = false;
	}

   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool lossRate::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}
