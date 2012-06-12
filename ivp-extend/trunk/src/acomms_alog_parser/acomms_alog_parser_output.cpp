/*
 * acomms_alog_parser_output.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: josh
 */


#include "acomms_alog_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// parsed data is stored in the vectors matched_transmit_events and leftover_receive_events
//
// matched_transmit_events contains all transmission events and receipts that have been
// matched to them.  If a receive event was not found for a node, sync loss is assumed.
//
// leftover_receive_events contains all receive events that could not be matched to a
// transmission event.  This may indicate you are missing log files or simply an error in
// the matcher.
//
// A list of all vehicle names can be found in the vector vehicle_names.  The vehicle names
// are treated as their unique identifiers, not the vehicle IDs.
//
// Significant features not currently implemented:
//   FSK Mini only transmissions (rate -1)
//   Receive status other than sync loss (2)
//   Non-ASCII character transmissions

void ACOMMS_ALOG_PARSER::outputResults() {
	// YOUR CODE HERE

	//Data
	/*for(int i=0;i<matched_transmit_events.size();i++){
		TRANSMISSION_EVENT te = matched_transmit_events[i];
		string my_node = te.transmitter_name;

			for(map<string,RECEPTION_EVENT>::iterator iter = te.receptions_map.begin(); iter != te.receptions_map.end(); ++iter){
			string rv =  iter->first;
			RECEPTION_EVENT re = iter->second;

			if(rv!=my_node){

				stringstream ss;
				ss<<"my_node"<<"_"<<rv<<"_logfile.txt";
				ifstream check_output(ss.str().c_str(), ifstream::in);
				ofstream log_output(ss.str().c_str());

				if(is_empty(check_output)){ //header
					log_output<<"Rate:"<<"Data:"
							<<"TGPS X:"<<"TGPS Y:" <<"TGPS Age:"<<"Transmission Time:"
							<<"RStatus:"<<"RTime:"<<"RStartTime:"<<"RGPS X:"<<"RGPS Y:"
							<<"RGPS Age:" <<"SNR In:" <<"SNR Out:" << "SPL" << endl;
				}

				log_output<<te.rate<<":"<<te.data<<":"<<te.gps_x<<":"
						<<te.gps_y<<":"<<te.gps_age<<":"<<te.transmission_time<<":";

				log_output<<re.receive_status<<":"<<re.receive_time<<":"<<re.receive_start_time
						<<":"<<re.gps_x<<":"<<re.gps_y<<":"<<re.gps_age<<":";

				int num_stats = re.data_msg.ExtensionSize( micromodem::protobuf::receive_stat );
				micromodem::protobuf::ReceiveStatistics stat;

				if ( num_stats == 1 ) {
					stat = re.data_msg.GetExtension( micromodem::protobuf::receive_stat, 0 );
				}
				else if ( num_stats == 2 ) {
					stat = re.data_msg.GetExtension( micromodem::protobuf::receive_stat, 1 );

					log_output<<stat.snr_in()<<":"<<stat.snr_out()<<":"<<stat.spl()<<endl;

					log_output.close();
					check_output.close();
				}
			}
		}
	}*/
}

bool ACOMMS_ALOG_PARSER::is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}
