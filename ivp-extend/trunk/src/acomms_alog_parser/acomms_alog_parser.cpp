/*
 * acomms_alog_parser.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */

#include "acomms_alog_parser.h"

using namespace std;
using namespace boost::posix_time;
using namespace lib_acomms_messages;

#define MAX_LINE_LENGTH 10000

#define COLOR_YELLOW "\33[33m"
#define COLOR_RED "\33[31m"
#define COLOR_CYAN "\33[36m"
#define COLOR_MAGENTA "\33[35m"
#define COLOR_GREEN "\33[32m"
#define COLOR_RESET "\33[0m"

int DIRE_WARNINGS = 0;
int ERRORS = 0;
bool use_hex = false;

map<ACOMMS_ALOG_PARSER::RECEPTION_EVENT::RECEIVE_STATUS,string> RECEIVE_STATUS_STRINGS;

ACOMMS_ALOG_PARSER::ACOMMS_ALOG_PARSER() {
	RECEIVE_STATUS_STRINGS[ACOMMS_ALOG_PARSER::RECEPTION_EVENT::not_set] = "not set";
	RECEIVE_STATUS_STRINGS[ACOMMS_ALOG_PARSER::RECEPTION_EVENT::received_fully] = "received fully";
	RECEIVE_STATUS_STRINGS[ACOMMS_ALOG_PARSER::RECEPTION_EVENT::bad_crcs] = "bad crcs";
	RECEIVE_STATUS_STRINGS[ACOMMS_ALOG_PARSER::RECEPTION_EVENT::sync_loss] = "sync loss";
	RECEIVE_STATUS_STRINGS[ACOMMS_ALOG_PARSER::RECEPTION_EVENT::driver_inactive] = "driver inactive";
}

// find the nearest entry by time, returns index and time diff
pair<int,double> findNearest( vector<double> time_vector, double msg_time ) {
	if ( time_vector.empty() ) {
		cout << "PASSED AN EMPTY VECTOR" << endl;
		return pair<int,double>(-1,-1);
	}
	double min_diff = 100000000;
	int index = -1;
	for ( int i=0; i<time_vector.size(); i++ ) {
		if ( fabs(time_vector[i]-msg_time) < min_diff ) {
			min_diff = fabs(time_vector[i]-msg_time);
			index = i;
		}
	}
	return pair<int,double>(index,min_diff);
}

// find the nearest entry by time, returns index and time diff
template <class T>
pair<int,double> findNearest( vector< pair<double,T> > item_list, double msg_time ) {
	if ( item_list.empty() ) {
		cout << "PASSED AN EMPTY VECTOR" << endl;
		return pair<int,double>(-1,-1);
	}
	double min_diff = 100000000;
	int index = -1;
	for ( int i=0; i<item_list.size(); i++ ) {
		if ( fabs(item_list[i].first-msg_time) < min_diff ) {
			min_diff = fabs(item_list[i].first-msg_time);
			index = i;
		}
	}
	return pair<int,double>(index,min_diff);
}

// get a range of values specified by time
template <class T>
vector< pair<double,T> > getRange(
		vector< pair<double,T> > item_list, double t_start, double t_end ) {

	vector< pair<double,T> > result;
	for ( int i=0; i<item_list.size(); i++ ) {
		if ( item_list[i].first > t_start && item_list[i].first < t_end ) {
			result.push_back( item_list[i] );
		}
	}
	return result;
}

template <class T>
int findValue ( vector<pair<double,T> > item_list, T value ) {
	for ( int i=0; i<item_list.size(); i++ ) {
		if ( item_list[i].second == value )
			return i;
	}
	return -1;
}

template <class T>
void applyOffset( vector<pair<double,T> > * item_list, double offset ) {
	for ( int i=0; i<item_list->size(); i++ ) {
		item_list->at(i).first += offset;
	}
}

template <class T>
bool general_sort( pair<double,T> obj1, pair<double,T> obj2 ) {
	return obj1.first < obj2.first;
}

template <class T>
void appendVector( vector<T> * append_to, vector<T> * append_from ) {
	for ( int i=0; i<append_from->size(); i++ ) {
		append_to->push_back( append_from->at(i) );
	}
}

string toNiceString( double secondstime ) {
	long secs = (long) secondstime;
	long millis = (long) (secondstime*1000) - secs*1000;
	time_duration td = seconds(secs) + milliseconds(millis);
	return to_simple_string(td);
}

// functions for sorting by time
bool transmission_sort ( ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT t1, ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT t2 ) {
	return t1.transmission_time < t2.transmission_time;
}
bool reception_sort ( ACOMMS_ALOG_PARSER::RECEPTION_EVENT r1, ACOMMS_ALOG_PARSER::RECEPTION_EVENT r2 ) {
	return r1.receive_time < r2.receive_time;
}

double ACOMMS_ALOG_PARSER::computeMatching(
		ACOMMS_ALOG_PARSER::RECEPTION_EVENT * r_event,
		ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT * t_event ) {

	// check time
	if ( r_event->receive_start_time - t_event->transmission_time > 10 ||
			t_event->transmission_time - r_event->receive_start_time > 2 ) {
		return -1;
	}

	double matching_score = 5;

	// more accurate timing
	double distance = sqrt( pow(r_event->gps_x - t_event->gps_x, 2.0) +
			pow(r_event->gps_y - t_event->gps_y, 2.0) );
	double expected_transit_time = distance/1500.0;
	double time_error = fabs(r_event->receive_start_time - t_event->transmission_time - expected_transit_time);
	matching_score += (10 - time_error);

	if ( use_hex ) {
		// compare frames
		if ( r_event->frame_status.size()==1 && r_event->frame_status[0]==1 ) {
			// for single frame packets we can just check
			if ( r_event->received_data_hex == t_event->transmitted_data_hex )
				matching_score+=2;
			else
				matching_score-=2;
		} else {
	//		vector<string> transmitted_frames;
	//		int total_size = t_event->transmitted_data_hex.
	//		while
		}
	}
}

// apply time offset to all saved variables and receptions/transmissions
void ACOMMS_ALOG_PARSER::FILE_INFO::applyTimeOffset( double offset ) {
	applyOffset( &gps_x, offset );
	applyOffset( &gps_y, offset );
//	applyOffset( &desired_thrust, offset );
//	applyOffset( &voltage, offset );
	applyOffset( &acomms_driver_status, offset );
	applyOffset( &acomms_transmit_data, offset );
	applyOffset( &acomms_transmit_data_binary, offset );
	applyOffset( &acomms_transmitted_data_hex, offset );
	applyOffset( &acomms_received_data_hex, offset );
	applyOffset( &gps_time, offset );
	applyOffset( &receptions, offset );
	applyOffset( &transmissions, offset );
	for ( int i=0; i<receptions.size(); i++ ) {
		receptions[i].second.receive_start_time += offset;
		receptions[i].second.receive_time += offset;
	}
	for ( int i=0; i<transmissions.size(); i++ ) {
		transmissions[i].second.transmission_time += offset;
	}
	map<string,vector<pair<double,double> > >::iterator double_iter;
	for ( double_iter=double_series.begin(); double_iter!=double_series.end(); ++double_iter ) {
		applyOffset( &(double_series[double_iter->first]), offset );
	}
	map<string,vector<pair<double,string> > >::iterator string_iter;
	for ( string_iter=string_series.begin(); string_iter!=string_series.end(); ++string_iter ) {
		applyOffset( &(string_series[string_iter->first]), offset );
	}
}

void ACOMMS_ALOG_PARSER::populateReceiveEvent( RECEPTION_EVENT * r_event, string name, double time ) {
	r_event->vehicle_name = name;
	r_event->source_filename = "artificial";

	pair<int,double> gps_x_pair = findNearest( gps_x[name], time );
	r_event->gps_x = gps_x[name][gps_x_pair.first].second;
	pair<int,double> gps_y_pair = findNearest( gps_y[name], time );
	r_event->gps_y = gps_y[name][gps_y_pair.first].second;
	r_event->gps_age = max(gps_x_pair.second, gps_y_pair.second);
}

void ACOMMS_ALOG_PARSER::runParser() {
	for ( int i=0; i<alog_files.size(); i++ ) {
		// pass in custom variables
		alog_files[i].double_vars = double_vars;
		alog_files[i].string_vars = string_vars;

		// perform initial processing
		// gets header lines, vehicle name, acomms ID, time offset, data, etc.
		alog_files[i].processFile();

		// get time offset of our log file and apply
		double time_offset = alog_files[i].getMOOSTimeOffset();
		alog_files[i].applyTimeOffset( time_offset );

		// add receipts to our list of receptions for this vehicle, now with absolute time
		for ( int j=0; j<alog_files[i].receptions.size(); j++ ) {
			RECEPTION_EVENT r_event = alog_files[i].receptions[j].second;
			vehicle_receptions[alog_files[i].getVehicleName()].push_back( r_event );
			all_receptions.push_back( r_event );
		}

		// add transmissions to our list of all recptions, now with absolute time
		for ( int j=0; j<alog_files[i].transmissions.size(); j++ ) {
			TRANSMISSION_EVENT t_event = alog_files[i].transmissions[j].second;
			vehicle_transmissions[alog_files[i].getVehicleName()].push_back(t_event);
			all_transmissions.push_back( t_event );
		}

		// add vehicle name to list of all vehicles if not already found
		if ( find( vehicle_names.begin(), vehicle_names.end(), alog_files[i].getVehicleName() ) == vehicle_names.end() ) {
			vehicle_names.push_back( alog_files[i].getVehicleName() );
		}

		// add individual data sets to full time history for that vehicle
		appendVector( &gps_x[alog_files[i].getVehicleName()], &alog_files[i].gps_x );
		appendVector( &gps_y[alog_files[i].getVehicleName()], &alog_files[i].gps_y );
//		appendVector( &desired_thrust[alog_files[i].getVehicleName()], &alog_files[i].desired_thrust );
//		appendVector( &voltage[alog_files[i].getVehicleName()], &alog_files[i].voltage );
		appendVector( &acomms_driver_status[alog_files[i].getVehicleName()], &alog_files[i].acomms_driver_status );
		appendVector( &acomms_transmit_data[alog_files[i].getVehicleName()], &alog_files[i].acomms_transmit_data );
		appendVector( &acomms_transmitted_data_hex[alog_files[i].getVehicleName()], &alog_files[i].acomms_transmitted_data_hex );
		appendVector( &acomms_received_data_hex[alog_files[i].getVehicleName()], &alog_files[i].acomms_received_data_hex );
		appendVector( &gps_time[alog_files[i].getVehicleName()], &alog_files[i].gps_time );

		map<string,vector<pair<double,double> > >::iterator double_iter;
		for ( 	double_iter=alog_files[i].double_series.begin();
				double_iter!=alog_files[i].double_series.end();
				++double_iter ) {
			appendVector( 	&(double_data[alog_files[i].getVehicleName()][double_iter->first]),
							&(alog_files[i].double_series[double_iter->first]) );
		}
		map<string,vector<pair<double,string> > >::iterator string_iter;
		for ( 	string_iter=alog_files[i].string_series.begin();
				string_iter!=alog_files[i].string_series.end();
				++string_iter ) {
			appendVector( 	&(string_data[alog_files[i].getVehicleName()][string_iter->first]),
							&(alog_files[i].string_series[string_iter->first]) );
		}
	}

	// for each vehicle, sort data by time
	for ( int i=0; i<vehicle_names.size(); i++ ) {
		string name = vehicle_names[i];
		sort( vehicle_receptions[name].begin(),
				vehicle_receptions[name].end(), reception_sort );
		sort( vehicle_transmissions[name].begin(),
				vehicle_transmissions[name].end(), transmission_sort );

		sort( gps_x[name].begin(), gps_x[name].end(), general_sort<double> );
		sort( gps_y[name].begin(), gps_y[name].end(), general_sort<double> );
//		sort( desired_thrust[name].begin(), desired_thrust[name].end(), general_sort<double> );
//		sort( voltage[name].begin(), voltage[name].end(), general_sort<double> );
		sort( acomms_driver_status[name].begin(), acomms_driver_status[name].end(), general_sort<string> );
		sort( acomms_transmit_data[name].begin(), acomms_transmit_data[name].end(), general_sort<string> );
		sort( acomms_transmitted_data_hex[name].begin(), acomms_transmitted_data_hex[name].end(), general_sort<string> );
		sort( acomms_received_data_hex[name].begin(), acomms_received_data_hex[name].end(), general_sort<string> );
		sort( gps_time[name].begin(), gps_time[name].end(), general_sort<boost::posix_time::ptime> );
	}

	// sort customizable data series separate because may be missing for certain vehicles
	map<string,map<string,vector<pair<double,double> > > >::iterator vditer;
	for( vditer=double_data.begin(); vditer!=double_data.end(); ++vditer ) {
		string name = vditer->first;
		map<string,vector<pair<double,double> > >::iterator diter;
		for ( diter=double_data[name].begin(); diter!=double_data[name].end(); ++diter ){
			string varname = diter->first;
			sort( 	double_data[name][varname].begin(),
					double_data[name][varname].end(),
					general_sort<double> );
		}
	}
	map<string,map<string,vector<pair<double,string> > > >::iterator vsiter;
	for ( vsiter=string_data.begin(); vsiter!=string_data.end(); ++vsiter ) {
		string name = vsiter->first;
		map<string,vector<pair<double,string> > >::iterator siter;
		for ( siter=string_data[name].begin(); siter!=string_data[name].end(); ++siter ) {
			string varname = siter->first;
			sort( 	string_data[name][varname].begin(),
					string_data[name][varname].end(),
					general_sort<string> );
		}
	}

	// sort all transmissions and receptions by time
	sort( all_transmissions.begin(), all_transmissions.end(), transmission_sort );
	sort( all_receptions.begin(), all_receptions.end(), reception_sort );

	// per vehicle
	for ( int i=0; i<vehicle_names.size(); i++ ) {
		cout << vehicle_names[i] << ": ";
		cout << vehicle_receptions[vehicle_names[i]].size() << " receptions,  ";
		cout << vehicle_transmissions[vehicle_names[i]].size() << " transmissions" << endl;
	}
	// total
	cout << all_receptions.size() << " total receptions" << endl;
	cout << all_transmissions.size() << " total transmissions" << endl;

	// summary of errors and dire warnings, if any
	if ( ERRORS > 0 || DIRE_WARNINGS > 0 ) {
		cout << COLOR_RED << DIRE_WARNINGS << " dire warnings and " << ERRORS <<
				" errors require attention." << COLOR_RESET << endl;
	}
	cout << COLOR_CYAN <<
			"File reading complete.  Press enter to begin matching."
			<< COLOR_RESET << endl;
	cin.get();
	cout << endl;

	vector<RECEPTION_EVENT> remaining_receptions = all_receptions;
	while ( !remaining_receptions.empty() ) {
		// fetch a reception
		RECEPTION_EVENT r_event = remaining_receptions.back();
		remaining_receptions.pop_back();
		string reception_name = r_event.vehicle_name;

		// compute matching scores with all transmissions
		vector<pair<double,int> > matching_scores;
		int best_index = -1;
		for ( int j=0; j<all_transmissions.size(); j++ ) {
			TRANSMISSION_EVENT t_event = all_transmissions[j];
			double this_score = computeMatching( &r_event, &t_event );
			matching_scores.push_back( pair<double,int>(this_score,j) );
		}

		// sort matching scores low to high
		sort(matching_scores.begin(), matching_scores.end(), general_sort<int>);

		// work backwards through list to find best
		for ( int j=matching_scores.size()-1; j>-1; j-- ) {
			if ( matching_scores[j].first < 0 ) {
				unmatched_receptions.push_back( r_event );
				break;
			}
			TRANSMISSION_EVENT * t_event = &all_transmissions[ matching_scores[j].second ];

			// check if transmission already has a reception on our vehicle
			int score_index = t_event->hasVehicleMatch( reception_name );

			if ( score_index != -1 ) {
				// already has, check if we're better
				if ( matching_scores[j].first > t_event->reception_matches[score_index].first ) {
					remaining_receptions.push_back( t_event->reception_matches[score_index].second );
					t_event->reception_matches.erase( t_event->reception_matches.begin()+=score_index );
					t_event->reception_matches.push_back( pair<double,RECEPTION_EVENT>
						( matching_scores[j].first, r_event ) );
					break;
				}
			} else {
				// doesn't have, add ourselves
				t_event->reception_matches.push_back( pair<double,RECEPTION_EVENT>
					( matching_scores[j].first, r_event ) );
				break;
			}
		}
	}

	int sync_losses =0;
	int num_vehicles = vehicle_names.size();
	for ( int i=0; i<all_transmissions.size(); i++ ) {
		TRANSMISSION_EVENT * t_event = &all_transmissions[i];
		for ( int j=0; j<vehicle_names.size(); j++ ) {
			string receiver_name = vehicle_names[j];
			if ( vehicle_names[j] != t_event->transmitter_name &&
					t_event->hasVehicleMatch( receiver_name ) == -1 ) {
				// no reception on a vehicle that isn't the transmitter

				// get driver status messages near time of transmission
				vector< pair<double,string> > statuses = getRange<string>(
						acomms_driver_status[receiver_name],
						t_event->transmission_time-5,
						t_event->transmission_time+5 );

				RECEPTION_EVENT r_event;
				populateReceiveEvent( &r_event, receiver_name, t_event->transmission_time );
				if ( statuses.empty() ) {
					// driver was not running
					r_event.receive_status = RECEPTION_EVENT::driver_inactive;
					r_event.debugInfo = "No status reports within 5 seconds.";
				} else if ( findValue<string>( statuses, "ready" ) == -1 ) {
					// driver was not ready
					r_event.receive_status = RECEPTION_EVENT::driver_inactive;
					stringstream ss;
					ss << "Driver was not ready: ";
					for ( int k=0; k<statuses.size(); k++ ) {
						ss << statuses[k].first << " @ " <<
								toNiceString( statuses[k].first );
						if ( k!=statuses.size()-1)
							ss << ", ";
					}
					r_event.debugInfo = ss.str();
				} else {
					// sync loss
					r_event.receive_status = RECEPTION_EVENT::sync_loss;
					sync_losses++;
				}

				t_event->reception_matches.push_back(
						pair<int,RECEPTION_EVENT> (-1, r_event) );
			}
		}
	}

	// populate receptions_vector and receptions_map
	for ( int i=0; i<all_transmissions.size(); i++ ) {
		TRANSMISSION_EVENT * t_event = &all_transmissions[i];
		for ( int j=0; j<t_event->reception_matches.size(); j++ ) {
			RECEPTION_EVENT r_event = t_event->reception_matches[j].second;
			t_event->receptions_vector.push_back( r_event );
			t_event->receptions_map[r_event.vehicle_name] = r_event;
		}
	}

	cout << all_transmissions.size() << " transmissions" << endl;
	cout << sync_losses << " sync losses" << endl;
	cout << unmatched_receptions.size() << " unmatched receptions" << endl;

	publishSummary();

	cout << COLOR_CYAN <<
			"Matching complete and summary published.  Press enter to run output."
			<< COLOR_RESET << endl;
	cin.get();
	cout << endl;

	outputResults();
}

// returns index of a vehicle in reception_matches if it exists, -1 otherwise
int ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT::hasVehicleMatch( string vehicle_name ) {
	int rc = -1;
	for ( int k=0; k<reception_matches.size(); k++ ) {
		if ( reception_matches[k].second.vehicle_name == vehicle_name ) {
			rc = k;
			break;
		}
	}
	return rc;
}

void ACOMMS_ALOG_PARSER::publishSummary() {
	ofstream summary;
	summary.open("summary.txt", ios::out);

	for ( int i=0; i<all_transmissions.size(); i++ ) {
		TRANSMISSION_EVENT t_event = all_transmissions[i];

		summary << "Transmit time: " << toNiceString( t_event.transmission_time ) << endl;
		summary << "Vehicle: " << t_event.transmitter_name << endl;
		summary << "Rate: " << t_event.rate << endl;
		summary << "Source id: " << t_event.source_id << endl;
		summary << "Data: " << t_event.data << endl;
		summary << "Hex data: " << t_event.transmitted_data_hex << endl;
		summary << "Number of receptions: " << t_event.receptions_vector.size() << endl;
		summary << endl;

		for ( int j=0; j<t_event.reception_matches.size(); j++ ) {
			RECEPTION_EVENT r_event = t_event.reception_matches[j].second;
			summary << "Vehicle: " << r_event.vehicle_name << endl;
			summary << "Receive status: " << RECEIVE_STATUS_STRINGS[ r_event.receive_status ] << endl;
			if ( !r_event.debugInfo.empty() )
				summary << "Debug info: " << r_event.debugInfo << endl;
			summary << "Receive start: " << toNiceString( r_event.receive_start_time ) << endl;
			summary << "Receive complete: " << toNiceString( r_event.receive_time ) << endl;
			double dist = sqrt( pow(r_event.gps_x-t_event.gps_x,2) +
					pow(r_event.gps_y-t_event.gps_y,2) );
			summary << "Distance from transmitter: " << dist;
			summary << "  age " << r_event.gps_age << endl;
			summary << "Matching score: " << t_event.reception_matches[j].first << endl;
			summary << "Rate: " << r_event.rate << endl;
			summary << "Source id: " << r_event.source_id << endl;
			summary << "Num frames: " << r_event.num_frames << endl;
			summary << "Frame status: ";
			for ( int k=0; k<r_event.frame_status.size(); k++ ) {
				summary << r_event.frame_status[k] << " ";
			} summary << endl;
			string data;
			for ( int k=0; k<r_event.data_msg.frame_size(); k++ ) {
				data += r_event.data_msg.frame(k);
			}
			summary << "Data: " << data << endl;
			summary << "Hex data: " << r_event.received_data_hex << endl;
			summary << endl;
		}

		summary << endl
				<< "-----------------------------------------------------------------------"
				<< endl << endl;
	}

	summary << endl << endl
			<< "-----------------------------------------------------------------------" << endl
			<< "------------------------UNMATCHED RECEPTIONS---------------------------" << endl
			<< "-----------------------------------------------------------------------" << endl
			<< endl << endl;

	for ( int i=0; i<unmatched_receptions.size(); i++ ) {
		RECEPTION_EVENT r_event = unmatched_receptions[i];
		summary << "Vehicle: " << r_event.vehicle_name << endl;
		summary << "Receive status: " << RECEIVE_STATUS_STRINGS[ r_event.receive_status ] << endl;
		if ( !r_event.debugInfo.empty() )
			summary << "Debug info: " << r_event.debugInfo << endl;
		summary << "Receive start: " << toNiceString( r_event.receive_start_time ) << endl;
		summary << "Receive complete: " << toNiceString( r_event.receive_time ) << endl;
		summary << "Rate: " << r_event.rate << endl;
		summary << "Source id: " << r_event.source_id << endl;
		summary << "Num frames: " << r_event.num_frames << endl;
		summary << "Frame status: ";
		for ( int k=0; k<r_event.frame_status.size(); k++ ) {
			summary << r_event.frame_status[k] << " ";
		} summary << endl;
		string data;
		for ( int k=0; k<r_event.data_msg.frame_size(); k++ ) {
			data += r_event.data_msg.frame(k);
		}
		summary << "Data: " << data << endl;
		summary << "Hex data: " << r_event.received_data_hex << endl;

		summary << endl
				<< "-----------------------------------------------------------------------"
				<< endl << endl;
	}

	summary.close();
}

// ------------------------------------------------------------------------------------------------

// intiial processing of an alog file
void ACOMMS_ALOG_PARSER::FILE_INFO::processFile() {
	cout << "Entering file: " << filename << endl;

//	cout << "opening " << filename << endl;
	openFile();

//	cout << "parsing moos file" << endl;
	// parse the associated moos file
	parseMOOSFile();
//	cout << "vehicle: " << vehicle_name << endl;
//	cout << "getting header lines" << endl;
	// parse header lines
	parseHeaderLines();
//	cout << "collecting data" << endl;
	collectData();

//	cout << "closing file" << endl;
	closeFile();

	// determine the time offset of this file
	if ( !offsetViaGPS() ) {
		cout << COLOR_YELLOW <<
				"WARNING: Failed to offset using gps, using system time from header."
				<< COLOR_RESET << endl;
		offsetViaHeader();
	}
	cout << "MOOSTime offset is: " << moos_time_offset << " (seconds from 12am)" << endl;

	// print some basic info about this log file
	cout << "vehicle: " << vehicle_name << "  transmissions: "
			<< transmissions.size() << "  receptions: " << receptions.size()
			<< endl << endl;
}

// force an alog entry to be a string
string fixToString( ALogEntry entry, int line_num ) {
	if ( entry.isNumerical() ) {
		cout << COLOR_YELLOW << "WARNING at line " << line_num <<
				": " << entry.getVarName() << " was numerical, will convert." << COLOR_RESET << endl;
		stringstream ss;
		ss << entry.getDoubleVal();
		return ss.str();
	} else {
		return entry.getStringVal();
	}
}

// main data collection function, all relevant variables need to picked up here
void ACOMMS_ALOG_PARSER::FILE_INFO::collectData() {
	ALogEntry entry = getNextEntry();
	int iteration = 0;

	// iterate over the entire file
	while ( entry.getStatus() != "eof" ) {
		iteration++;
		int line_num = iteration+5; // 5 header lines
//		if ( iteration%1000 == 0 ) {
//			cout << entry.getTimeStamp() << endl;
//		}
		if ( entry.getStatus() != "invalid" ) {
			string key = entry.getVarName();
			double msg_time = entry.getTimeStamp();

			if ( key == "GPS_X" || key == "NAV_X" ) {
				gps_x.push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( key == "GPS_Y" || key == "NAV_Y" ) {
				gps_y.push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( key == "GPS_PTIME" ) {
				try {
					gps_time.push_back( pair<double,ptime>( msg_time,
							time_from_string( entry.getStringVal() ) ) );
				} catch ( exception & e ) {
//					cout << "failed to parse gps_ptime: " << entry.getStringVal() << endl;
				}
//			} else if ( key == "DESIRED_THRUST" ) {
//				desired_thrust.push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( key == "ACOMMS_DRIVER_STATUS" ) {
				acomms_driver_status.push_back( pair<double,string>( msg_time, entry.getStringVal() ) );
			} else if ( key == "ACOMMS_TRANSMIT_DATA" ) {
				acomms_transmit_data.push_back( pair<double,string>( msg_time,
						fixToString( entry, line_num) ) );
			} else if ( key == "ACOMMS_RECEIVED_ALL" ) {
//				unprocessed_receipts.push_back( pair<double,string>( iteration, entry.getStringVal() ) );
				receptions.push_back( pair<double, RECEPTION_EVENT>( msg_time,
						constructReception( msg_time, line_num, entry.getStringVal() ) ) );
			} else if ( key == "ACOMMS_TRANSMIT_SIMPLE" ) {
//				unprocessed_transmissions.push_back( pair<double,string>( line_num, entry.getStringVal() ) );
				transmissions.push_back( pair<double, TRANSMISSION_EVENT>( msg_time,
						constructTransmission( msg_time, line_num, entry.getStringVal() ) ) );
//			} else if ( key == "VOLTAGE" ) {
//				voltage.push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( key == "ACOMMS_TRANSMITTED_DATA_HEX" ) {
				acomms_transmitted_data_hex.push_back( pair<double,string>( msg_time,
						fixToString( entry, line_num ) ) );
			} else if ( key == "ACOMMS_RECEIVED_DATA_HEX" ) {
				acomms_received_data_hex.push_back( pair<double,string>( msg_time,
						fixToString( entry, line_num ) ) );
			} else if ( find(double_vars.begin(), double_vars.end(), key) != double_vars.end() ) {
				double_series[key].push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( find(string_vars.begin(), string_vars.end(), key) != string_vars.end() ) {
				string_series[key].push_back( pair<double,string>( msg_time,
						fixToString( entry, line_num ) ) );
			}
		}

		entry = getNextEntry();
	}

	// check for partial receives and merge
	if ( receptions.size() > 1 ) {
		if ( receptions[0].second.missing_stats ) {
			tryCombine(0, 1);
		} else if ( receptions[receptions.size()-1].second.missing_stats ) {
			tryCombine(receptions.size()-1, receptions.size()-2);
		}
	} else if ( receptions.size() > 2 ) {
		for ( int i=1; i<receptions.size()-1; i++ ) {
			if ( receptions[i].second.missing_stats ) {
				if ( !tryCombine(i,i+1) ) tryCombine(i,i-1);
			}
		}
	}

	// clean up any partial receives
	for ( int i=0; i<receptions.size(); i++ ) {
		if ( receptions[i].second.missing_frames ) {
			cout << COLOR_YELLOW << "WARNING at line " << receptions[i].second.source_line <<
					": failed to find frames for these statistics, deleting." <<
					COLOR_RESET << endl;
			receptions.erase(receptions.begin()+=i);
		} else if ( receptions[i].second.missing_stats ) {
			cout << COLOR_YELLOW << "WARNING at line " << receptions[i].second.source_line <<
					": failed to find statistics for these frames, deleting." <<
					COLOR_RESET << endl;
			receptions.erase(receptions.begin()+=i);
		}
	}

	// finish constructing receptions
	// check for no receive hex data in log file
	if ( acomms_received_data_hex.empty() && !receptions.empty() ) {
		cout << COLOR_YELLOW <<
				"WARNING: this log file does not appear to contain hex formatted receive data"
				<< COLOR_RESET << endl;
	} else {
		for ( int i=0; i<receptions.size(); i++ ) {
			// check if receive time is reasonable
			RECEPTION_EVENT * r_event = &receptions[i].second;
			double receive_length = r_event->receive_time -r_event->receive_start_time;
			// negative is bad
			if ( receive_length < 0 ) {
				cout << COLOR_RED << "ERROR at line " << r_event->source_line <<
						": receive length is " << receive_length << "." << COLOR_RESET << endl;
			} else if ( receive_length > 7 ) {
				// manually set too big of times based on rate
				bool success = true;
				switch ( r_event->rate ) {
				case 0:
					r_event->receive_start_time = r_event->receive_time - FSK0_RECEIVE_DURATION;
					break;
				case 1:
					r_event->receive_start_time = r_event->receive_time - PSK1_RECEIVE_DURATION;
					break;
				case 2:
					r_event->receive_start_time = r_event->receive_time - PSK2_RECEIVE_DURATION;
					break;
				case 100:
					r_event->receive_start_time = r_event->receive_time - MINI_RECEIVE_DURATION;
					break;
				default:
					success = false;
					break;
				}
				if ( success ) {
					cout << COLOR_GREEN << "Manually set receive start time at line " <<
							r_event->source_line << " to " << r_event->receive_start_time
							<< "." << COLOR_RESET << endl;
				} else {
					cout << COLOR_RED << "ERROR manually setting receive start time at line " <<
							r_event->source_line << ": unhandled rate " << r_event->rate << "." <<
							COLOR_RESET << endl;
					ERRORS++;
				}
			}

			// if there are frames, look for hex posting
			if ( r_event->data_msg.frame_size() > 0 ) {
				// find the hex data nearest to our time
				pair<int,double> hex_index = findNearest( acomms_received_data_hex, receptions[i].first );
				if ( hex_index.first == -1 ) { // failed to find
					cout << COLOR_YELLOW << "WARNING at line " << r_event->source_line
							<< ": failed to find hex data for reception." << COLOR_RESET << endl;
				} else if ( hex_index.second > 1 ) { // time not very close
						cout << COLOR_YELLOW << "WARNING at line " << r_event->source_line
								<< ": found receive hex data " << hex_index.second
								<< " seconds away - not setting." << COLOR_RESET << endl;
				} else { // all good
					r_event->received_data_hex =
							acomms_received_data_hex[hex_index.first].second;
				}
			}
		}
	}

	// finish constructing transmissions
	// check for no transmit hex data in log file
	if ( acomms_transmitted_data_hex.empty() && !transmissions.empty() ) {
		cout << COLOR_YELLOW <<
				"WARNING: this log file does not appear to contain hex formatted transmit data"
				<< COLOR_RESET << endl;
	} else {
		for ( int i=0; i<transmissions.size(); i++ ) {
			pair<int,double> hex_index = findNearest( acomms_transmitted_data_hex, transmissions[i].first );
			if ( hex_index.first == -1 ) { // failed to find
				cout << COLOR_YELLOW << "WARNING at line " << transmissions[i].second.source_line
						<< ": failed to find hex data for transmission." << COLOR_RESET << endl;
			} else if ( hex_index.second > 1 ) { // time not very close
				cout << COLOR_YELLOW << "WARNING at line " << transmissions[i].second.source_line
						<< ": found transmit hex data " << hex_index.second
						<< " seconds away - deleting transmission (perhaps unhandled rate?)." << COLOR_RESET << endl;
				transmissions.erase( transmissions.begin()+=i );
			} else { // all good
				transmissions[i].second.transmitted_data_hex =
						acomms_transmitted_data_hex[hex_index.first].second;
			}
		}
	}
}

bool ACOMMS_ALOG_PARSER::FILE_INFO::tryCombine( int frame_index, int stats_index ) {
	if ( receptions[stats_index].second.missing_frames ) {
		fillStats( &receptions[frame_index].second, receptions[stats_index].second );
		cout << COLOR_GREEN << "Merged receive statistics at line " <<
				receptions[stats_index].second.source_line << " with frames at line " <<
				receptions[frame_index].second.source_line << ".  New protobuf: " << endl;
		cout << receptions[frame_index].second.data_msg.DebugString()
				<< COLOR_RESET << endl;
		receptions.erase(receptions.begin()+=stats_index);
		receptions[frame_index].second.missing_stats = false;

		goby::acomms::protobuf::ModemTransmission trans =
				receptions[frame_index].second.data_msg;
		micromodem::protobuf::ReceiveStatistics stat;
		int num_stats = trans.ExtensionSize(micromodem::protobuf::receive_stat);

		// find the relevant receive statistics
		if( num_stats == 1 ) { // psk or mini packet transmission
			stat = trans.GetExtension( micromodem::protobuf::receive_stat, 0 );
		} else if ( num_stats == 2 ) { // fsk transmission
			stat = trans.GetExtension( micromodem::protobuf::receive_stat, 1 );
		} else {
			cout << COLOR_RED << "ERROR combining frames at " <<
					receptions[frame_index].second.source_line << " with stats at " <<
					receptions[stats_index].second.source_line << ": " <<
					num_stats << " resulting receive statistics." << COLOR_RESET << endl;
			return false;
		}

		// do delayed stats processing
		processStatistics( &receptions[frame_index].second, stat,
			receptions[frame_index].second.source_line );

		return true;
	} else {
		return false;
	}
}

// use receive statistics from stats_event to fill frame_event
void ACOMMS_ALOG_PARSER::FILE_INFO::fillStats(
		ACOMMS_ALOG_PARSER::RECEPTION_EVENT * frame_event,
		ACOMMS_ALOG_PARSER::RECEPTION_EVENT stats_event ) {
	for ( int i=0; i<stats_event.data_msg.ExtensionSize(
			micromodem::protobuf::receive_stat); i++ ) {
	    micromodem::protobuf::ReceiveStatistics* cst =
	    		frame_event->data_msg.AddExtension(micromodem::protobuf::receive_stat);
	    cst->CopyFrom( stats_event.data_msg.GetExtension( micromodem::protobuf::receive_stat, i ) );
	}
}

// construct a reception event from a ACOMMS_RECEIVED_ALL log entry
ACOMMS_ALOG_PARSER::RECEPTION_EVENT ACOMMS_ALOG_PARSER::FILE_INFO::constructReception( double msg_time, int line_number, string msg_val ) {
//	cout << msg_time << endl;
//	cout << msg_val << endl;
	RECEPTION_EVENT r_event;

	// file information
	r_event.source_filename = filename;
	r_event.source_line = line_number;

	// vehicle information
	r_event.vehicle_name = vehicle_name;
	r_event.vehicle_id = vehicle_id;

	// time of message
	r_event.receive_time = msg_time;

	// reconstruct the protobuf
	// check for bad time in receive stats
	if ( msg_val.find("time: \"") != string::npos ) {
		cout << COLOR_GREEN << "FIXING at line " << line_number <<
				": bad time in received_all - attempting to correct" << COLOR_RESET << endl;
		// position of bad time in receive statistics
		int bad_time_index = msg_val.find("time: \"");
		int bad_time_end_index = msg_val.find("<|>",bad_time_index)-1;

		// position of good time in modem transmission
		int good_time_index = msg_val.find("time: ");
		int good_time_end_index = msg_val.find("<|>",good_time_index)-1;

		// make replacement
		string good_time = msg_val.substr(good_time_index, good_time_end_index-good_time_index+1);
		msg_val.replace(bad_time_index, bad_time_end_index-bad_time_index+1,
				good_time);
	}
	// put line endings back in
	while ( msg_val.find("<|>") != string::npos ) {
		msg_val.replace( msg_val.find("<|>"), 3, "\n" );
	}
	// parse protobuf
	string const* ptr = &msg_val;
	if ( !google::protobuf::TextFormat::ParseFromString( *ptr, &r_event.data_msg ) ) {
		cout << COLOR_RED << "ERROR parsing protobuf at line " << line_number
				<< " when attempting to parse: " << COLOR_RESET << endl;
		cout << COLOR_RED << msg_val << COLOR_RESET << endl;
		ERRORS++;
		// protobuf should show details of error on previous line
	}

	// most recent gps information
	if ( gps_x.size() > 0 ) {
		r_event.gps_x = gps_x.back().second;
		r_event.gps_y = gps_y.back().second;
		r_event.gps_age = r_event.receive_time-gps_x.back().first;
	}

	// backwards search from receive time to find beginning of receipt
	r_event.receive_start_time = -10;
	for ( int i=acomms_driver_status.size()-1; i>=0; i-- ) {
		if ( acomms_driver_status[i].second == "receiving" ) {
			r_event.receive_start_time = acomms_driver_status[i].first;
			break;
		}
	}

	// get the modem transmission and receive statistics
	goby::acomms::protobuf::ModemTransmission trans = r_event.data_msg;
	micromodem::protobuf::ReceiveStatistics stat;
	int num_stats = trans.ExtensionSize(micromodem::protobuf::receive_stat);

	// find the relevant receive statistics
	if( num_stats == 1 ) { // psk or mini packet transmission
		stat = trans.GetExtension( micromodem::protobuf::receive_stat, 0 );
	} else if ( num_stats == 2 ) { // fsk transmission
		stat = trans.GetExtension( micromodem::protobuf::receive_stat, 1 );
	} else if ( num_stats > 2 ) {
		cout << COLOR_RED << "ERROR at line " << line_number <<
				": protobuf has " << num_stats
				<< " receive statistics." << COLOR_RESET << endl;
		ERRORS++;
		return r_event;
	} else if ( num_stats == 0 ) {
		cout << COLOR_YELLOW << "WARNING at line " << line_number <<
				": protobuf has no receive statistics, will look later.  Statistics processing postponed"
				<< COLOR_RESET << endl;
		r_event.missing_stats = true;
		return r_event;
	}

	// check for missing frames
	int num_bad_frames;
	if ( stat.psk_error_code() == micromodem::protobuf::BAD_CRC_DATA_HEADER ||
			stat.psk_error_code() == micromodem::protobuf::BAD_MODULATION_HEADER ||
			stat.psk_error_code() == micromodem::protobuf::MISSED_START_OF_PSK_PACKET   ) {
		num_bad_frames = stat.number_frames();
	} else {
		num_bad_frames = stat.number_bad_frames();
	}
	if ( stat.number_frames() - num_bad_frames > 0 && trans.frame_size() == 0 ) {
		// missing frames - isolated receive statistics
		cout << COLOR_YELLOW << "WARNING at line " << line_number <<
				": receive statistics missing frames, will look later.  Statistics processing postponed."
				<< COLOR_RESET << endl;
		r_event.missing_frames = true;
		return r_event;
	}

	processStatistics( &r_event, stat, line_number );

//	cout << "parsed okay" << endl;
	return r_event;
}

void ACOMMS_ALOG_PARSER::FILE_INFO::processStatistics( ACOMMS_ALOG_PARSER::RECEPTION_EVENT * r_event,
		micromodem::protobuf::ReceiveStatistics stat, int line_number ) {

	goby::acomms::protobuf::ModemTransmission trans = r_event->data_msg;

	// number of frames an source id
	r_event->num_frames = stat.number_frames();
	r_event->source_id = stat.source();

	// check for mini packets and determine rate
	if ( trans.type() == goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC ) {
		if ( trans.HasExtension( micromodem::protobuf::type ) ) {
			micromodem::protobuf::TransmissionType type = trans.GetExtension( micromodem::protobuf::type );
			if ( type == micromodem::protobuf::MICROMODEM_MINI_DATA )
				r_event->rate = 100;
			else {
				cout << COLOR_RED << "ERROR at line " << line_number <<
						": unrecognized transmission type " << type <<
						COLOR_RESET << endl;
				ERRORS++;
				r_event->rate = -1;
			}
		} else {
			cout << COLOR_RED << "ERROR at line " << line_number <<
					": missing driver specific transmission type extension" <<
					COLOR_RESET << endl;
			ERRORS++;
			r_event->rate = -1;
		}
	} else {
		r_event->rate = stat.rate();
	}

	if ( trans.frame_size() == 0 ) {
		for ( int i=0; i<r_event->num_frames; i++ ) {
			r_event->frame_status.push_back(2);
		}
	} else {
		for ( int i=0; i<r_event->num_frames; i++ ) {
			r_event->frame_status.push_back(1);
		}
		int bad_frames = trans.ExtensionSize( micromodem::protobuf::frame_with_bad_crc );
		for ( int i=0; i<bad_frames; i++ ) {
			int bad_index = trans.GetExtension( micromodem::protobuf::frame_with_bad_crc, i );
			if ( bad_index >= r_event->frame_status.size() || bad_index < 0 ) {
				cout << COLOR_RED << "ERROR at line " << line_number <<
						": bad frame reported at position " << bad_index <<
						" but only " << r_event->num_frames << " total." <<
						COLOR_RESET << endl;
				ERRORS++;
			} else {
				r_event->frame_status[bad_index] = 2;
			}
		}
	}
}

ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT ACOMMS_ALOG_PARSER::FILE_INFO::constructTransmission( double msg_time, int line_number, string msg_val ) {
//	cout << msg_time << endl;
//	cout << msg_val << endl;
	TRANSMISSION_EVENT t_event;

	// file information
	t_event.source_filename = filename;
	t_event.source_line = line_number;

	// vehicle info
	t_event.transmitter_name = vehicle_name;
	t_event.source_id = vehicle_id;

	// time of message
	t_event.transmission_time = msg_time;

	// look for most recent gps info
	if ( gps_x.size() > 0 ) {
		t_event.gps_x = gps_x.back().second;
		t_event.gps_y = gps_y.back().second;
		t_event.gps_age = t_event.transmission_time - gps_x.back().first;
	}

	// look for data that was transmitted
	if ( acomms_transmit_data.size()!=0) {
		t_event.data = acomms_transmit_data.back().second;
		// check time of data
		if ( msg_time - acomms_transmit_data.back().first > 5 ) {
			cout << COLOR_YELLOW << "WARNING at line " << line_number <<
					": transmit data " << msg_time-acomms_transmit_data.back().first <<
					" seconds old." << COLOR_RESET << endl;
		}
	}
	// check for overlapping data transmissions
	if ( acomms_transmit_data.size() > 1 ) {
		if ( acomms_transmit_data.back().first -
				acomms_transmit_data[acomms_transmit_data.size()-2].first < 5 ) {
			cout << COLOR_YELLOW << "WARNING at line " << line_number <<
					": two transmit data's within 5 seconds of each other." << COLOR_RESET << endl;
		}
	}

	// parse transmit info from msg for rate and destination
	SIMPLIFIED_TRANSMIT_INFO ats( msg_val );
	t_event.rate = ats.rate;
	t_event.destination_id = ats.dest;
	t_event.num_frames = ats.num_frames;

//	cout << "parsed okay" << endl;

	return t_event;
}

void ACOMMS_ALOG_PARSER::FILE_INFO::openFile() {
//	cout << "opening " << filename << endl;
	logfile = fopen( filename.c_str(), "r" );
	file_is_open = true;
//	cout << "fetching header" << endl;
//	parseHeaderLines();
}

void ACOMMS_ALOG_PARSER::FILE_INFO::parseHeaderLines() {
	// pull first 5 lines from logfile (middle 3 are headers)
	getNextLine();
	header_lines.clear();
	for ( int i=0; i<3; i++ )
		header_lines.push_back( getNextLine() );
	getNextLine();
}

void ACOMMS_ALOG_PARSER::FILE_INFO::closeFile() {
	fclose(logfile);
	file_is_open = false;
}

ALogEntry ACOMMS_ALOG_PARSER::FILE_INFO::getNextEntry() {
	if ( !file_is_open )
		openFile();
	return getNextRawALogEntry_josh( logfile );
}

string ACOMMS_ALOG_PARSER::FILE_INFO::getNextLine() {
	if ( !file_is_open )
		openFile();
	return getNextRawLine_josh( logfile );
}

void ACOMMS_ALOG_PARSER::FILE_INFO::parseMOOSFile() {
	// find the moos file by replacing file extension
	string moosfilename = filename;
	int extension_index = moosfilename.find(".alog");
	moosfilename.replace(extension_index, 5, "._moos");

	// construct a mission reader to read our moos file
	CProcessConfigReader missionreader;
	missionreader.SetFile( moosfilename );

	// get acomms ID and vehicle name (community name)
	missionreader.GetConfigurationParam("iacomms_driver", "ID", vehicle_id);
	missionreader.GetValue("Community", vehicle_name);
}

void ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaHeader() {
	// parse date out of the header
	int date_start = header_lines[1].find("ON ") + 3;
	string date_string = header_lines[1].substr(date_start, header_lines[1].size()-date_start);
	if ( date_string[0] == ' ' )
		date_string.erase(date_string.begin());
//	cout << date_string << endl;
	tm header_time;
	strptime( date_string.c_str(), "%a %b %d %H:%M:%S %Y", &header_time);
//	cout << asctime( &header_time ) << endl;
	creation_time = boost::posix_time::ptime_from_tm( header_time );
//	cout << boost::posix_time::to_simple_string( creation_time ) << endl;

//	ALogEntry entry = getNextRawALogEntry( logfile );
	// convert to seconds (since 12am)
	moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0;// - entry.time();
}

bool ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaGPS() {
	if ( gps_time.size() < 2 ) {
		return false;
	} else {
		double sum = 0;
		int iterations = 0;

		for ( int i=1; i<gps_time.size(); i+=5 ) {
			iterations++;
			// parse ptime and apply time zone correction
			ptime first_gps_ptime = gps_time[1].second;
			time_duration utc_correction = hours(UTC_TIME_OFFSET);
			first_gps_ptime += utc_correction;

			// time at which that gps time was received, seconds and milliseconds
			int first_gps_moos_time_seconds = floor( gps_time[1].first );
			int first_gps_moos_time_milliseconds = floor( gps_time[1].first*1000.0) - first_gps_moos_time_seconds*1000;
			// convert to time duration
			time_duration first_gps_td = seconds ( first_gps_moos_time_seconds ) +
					milliseconds( first_gps_moos_time_milliseconds );

	//		cout << "first gps time " << first_gps_td << endl;
	//		cout << "ptime " << first_gps_ptime << endl;

			// time 0 in log file = this gps time - time since 0
			creation_time = first_gps_ptime - first_gps_td;// - first_moos_time_td;
			// convert to seconds - time since 12am that day
			sum += creation_time.time_of_day().total_milliseconds()/1000.0;// -first_moos_time;
		}
		moos_time_offset = sum/((double) iterations);

		return true;
	}
}

// ------------------------------------------------------------------------------------------------

void ACOMMS_ALOG_PARSER::addAlogFile( std::string filename ) {
	alog_files.push_back( FILE_INFO( filename ) );
}

void ACOMMS_ALOG_PARSER::addAlogFile( boost::filesystem::path filepath ) {
	// AVERT THINE EYES
	alog_files.push_back( FILE_INFO( string( filepath.string().c_str() ) ) );
}

string ACOMMS_ALOG_PARSER::getNextRawLine_josh(FILE *fileptr)
{
  if(!fileptr) {
    cout << "failed getNextLine() - null file pointer" << endl;
    return("err");
  }

  bool   EOL     = false;
  int    buffix  = 0;
  int    myint   = '\0';
  char   buff[MAX_LINE_LENGTH];

  int iteration = 0;
  while((!EOL) && (buffix < MAX_LINE_LENGTH)) {
	  iteration++;
//	cout << iteration << endl;
    myint = fgetc(fileptr);
//    cout << "got that character" << endl;
    unsigned char mychar = myint;
    switch(myint) {
    case EOF:
      fclose(fileptr);
      fileptr = 0;
      return("eof");
    case '\n':
      buff[buffix] = '\0';  // attach terminating NULL
      EOL = true;
      break;
    default:
      buff[buffix] = mychar;
      buffix++;
    }
  }
  string str = buff;
  return(str);
}

ALogEntry ACOMMS_ALOG_PARSER::getNextRawALogEntry_josh(FILE *fileptr, bool allstrings)
{
	ALogEntry entry;
	if (!fileptr) {
		cout << "failed getNextRawALogEntry() - null file pointer" << endl;
		entry.setStatus("invalid");
		return (entry);
	}

	bool EOLine = false;
	bool EOFile = false;
	int buffix = 0;
	int lineix = 0;
	int myint = '\0';
	char buff[MAX_LINE_LENGTH];

	string time, var, rawsrc, val;

	// Simple state machine:
	//   0: time
	//   1: between time and variable
	//   2: variable
	//   3: between variable and source
	//   4: source
	//   5: between source and value
	//   6: value
	int state = 0;

	while ((!EOLine) && (!EOFile) && (lineix < MAX_LINE_LENGTH)) {
//		cout << "state: " << state << endl;
		myint = fgetc(fileptr);
		unsigned char mychar = myint;
		switch (myint) {
		case EOF:
			EOFile = true;
			break;
		case ' ':
			if (state == 6) {
				buff[buffix] = mychar;
				buffix++;
			}
//			break;
		case '\t':
			if (state == 0) {
				buff[buffix] = '\0';
				time = buff;
				buffix = 0;
				state = 1;
			} else if (state == 2) {
				buff[buffix] = '\0';
				var = buff;
				buffix = 0;
				state = 3;
			} else if (state == 4) {
				buff[buffix] = '\0';
				rawsrc = buff;
				buffix = 0;
				state = 5;
			}
			break;
		case '\n':
			buff[buffix] = '\0'; // attach terminating NULL
			val = buff;
			EOLine = true;
			break;
		default:
			if (state == 1)
				state = 2;
			else if (state == 3)
				state = 4;
			else if (state == 5)
				state = 6;
			buff[buffix] = mychar;
			buffix++;
		}
		lineix++;
	}

	string src = biteString(rawsrc, ':');
	string srcaux = rawsrc;

	val = stripBlankEnds(val);

//	cout << "t:" << time << " v:" << var << " s:" << src << " v:" << val << endl;

	if ((time != "") && (var != "") && (src != "") && (val != "")
			&& isNumber(time)) {
		if (allstrings || !isNumber(val))
			entry.set(atof(time.c_str()), var, src, srcaux, val);
		else
			entry.set(atof(time.c_str()), var, src, srcaux, atof(val.c_str()));
	} else {
		if (EOFile)
			entry.setStatus("eof");
		else
			entry.setStatus("invalid");
	}

	return (entry);
}
