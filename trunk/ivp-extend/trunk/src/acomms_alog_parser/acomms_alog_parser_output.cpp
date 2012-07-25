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

	vector<string> vars;
	map<string,int> output_files;
	char delim = ',';

	//Acomms Stats
	vars.push_back("rate");
	vars.push_back("tgps_x");
	vars.push_back("tgps_y");
	vars.push_back("tgps_age");
	vars.push_back("transmission_time");
	vars.push_back("receive_status");
	vars.push_back("receive_time");
	vars.push_back("rgps_x");
	vars.push_back("rgps_y");
	vars.push_back("rgps_age");
	vars.push_back("snr_in");
	vars.push_back("snr_out");
	vars.push_back("spl");
	vars.push_back("mse");
	vars.push_back("noise");

	//Other
	//vars.push_back("relay_update");
	//vars.push_back("relay_update_age");
	//vars.push_back("relay_index_list");
	//vars.push_back("relay_index_list_age");

	for(int i=0;i<all_transmissions.size();i++){
		TRANSMISSION_EVENT te = all_transmissions[i];
		string my_node = te.transmitter_name;

		for(int i=0;i<te.receptions_vector.size();i++){
			string rv =  te.receptions_vector[i].vehicle_name;
			string dstr = '_' + rv;
			RECEPTION_EVENT re = te.receptions_vector[i];

			stringstream ss;
			ss<<my_node<<"_"<<rv<<"_logfile.txt";

			output_files[ss.str()]++;

			ofstream log_output;
			log_output.open(ss.str().c_str(),ios::app);

			if(output_files[ss.str()]==1){

				for(int i=0;i<vars.size();i++){
					log_output << vars[i] << dstr;
					if(i < vars.size()-1) {
						log_output << delim;
					}
				}
				log_output << endl;
				output_files[ss.str()]++;
			}

			log_output<<te.rate<<delim<<te.gps_x<<delim<<te.gps_y<<delim<<te.gps_age<<delim<<te.transmission_time<<delim;
			log_output<<re.receive_status<<delim<<re.receive_time<<delim<<re.gps_x<<delim<<re.gps_y<<delim<<re.gps_age<<delim;

			int num_stats = re.data_msg.ExtensionSize( micromodem::protobuf::receive_stat );
			micromodem::protobuf::ReceiveStatistics stat;

			if ( num_stats == 1 ) {
				stat = re.data_msg.GetExtension( micromodem::protobuf::receive_stat, 0 );
			}
			else if ( num_stats == 2 ) {
				stat = re.data_msg.GetExtension( micromodem::protobuf::receive_stat, 1 );
			}
			log_output<<stat.snr_in()<<delim<<stat.snr_out()<<delim<<stat.spl()<<delim
					<<stat.mse_equalizer()<<delim<<stat.stddev_noise();

			if(rv == "false"){
				cout << "Trying to find relay stats" << endl;
				vector< pair<double,string> > relay_stats, relay_index;
				relay_stats = string_data[rv]["SEARCH_RELAY_STATS"];
				relay_index = string_data[rv]["SEARCH_RELAY_INDEX_LIST"];

				log_output << delim;
				pair<int,double> temp = findNearest(relay_stats,re.receive_time);
				cout << temp.first << endl;

				if(temp.first>=0){
				log_output << relay_stats[temp.first].second << delim;
				log_output << temp.second << delim;
				temp = findNearest(relay_index,re.receive_time);
				log_output << relay_index[temp.first].second << delim;
				log_output << temp.second;
				}
			}

			log_output << endl;
			log_output.close();

		}
	}
}

