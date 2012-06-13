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

ACOMMS_ALOG_PARSER::ACOMMS_ALOG_PARSER() {
}

bool transmission_sort ( ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT t1, ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT t2 ) {
	return t1.transmission_time < t2.transmission_time;
}

bool reception_sort ( ACOMMS_ALOG_PARSER::RECEPTION_EVENT r1, ACOMMS_ALOG_PARSER::RECEPTION_EVENT r2 ) {
	return r1.receive_time < r2.receive_time;
}

void ACOMMS_ALOG_PARSER::runParser() {
	for ( int i=0; i<alog_files.size(); i++ ) {
		alog_files[i].processFile();
		double time_offset = alog_files[i].getMOOSTimeOffset();

		for ( int j=0; j<alog_files[i].receptions.size(); j++ ) {
			RECEPTION_EVENT r_event = alog_files[i].receptions[j].second;
			r_event.receive_time += time_offset;
			r_event.receive_start_time += time_offset;
			all_receptions[alog_files[i].getVehicleName()].push_back( r_event );
		}
		for ( int j=0; j<alog_files[i].transmissions.size(); j++ ) {
			TRANSMISSION_EVENT t_event = alog_files[i].transmissions[j].second;
			t_event.transmission_time += time_offset;
			all_transmissions.push_back( t_event );
		}

		if ( find( vehicle_names.begin(), vehicle_names.end(), alog_files[i].getVehicleName() ) == vehicle_names.end() ) {
			vehicle_names.push_back( alog_files[i].getVehicleName() );
		}
	}

	for ( int i=0; i<vehicle_names.size(); i++ ) {
		sort( all_receptions[vehicle_names[i]].begin(),
				all_receptions[vehicle_names[i]].end(), reception_sort );
	}

	sort( all_transmissions.begin(), all_transmissions.end(), transmission_sort );



	int sum = 0;
	for ( int i=0; i<vehicle_names.size(); i++ ) {
		sum+=all_receptions[vehicle_names[i]].size();
	}
	cout << sum << " available receptions" << endl;

	int sync_losses = 0;
	for ( int i=0; i<all_transmissions.size(); i++ ) {
		TRANSMISSION_EVENT t_event = all_transmissions[i];
//		cout << "transmission at time " << t_event.transmission_time << endl;
		for ( int j=0; j<vehicle_names.size(); j++ ) {
			string name = vehicle_names[j];
			bool reception_found = false;
			for ( int k=0; k<all_receptions[name].size(); k++ ) {
				RECEPTION_EVENT r_event = all_receptions[name][k];
//				cout << "reception at time " << r_event.receive_time << endl;
				double time_diff = r_event.receive_time - t_event.transmission_time;
				if ( time_diff < 10 && time_diff > 0 ) {
					t_event.receptions_map[name] = r_event;
					t_event.receptions_vector.push_back( r_event );
					reception_found = true;
					all_receptions[name].erase( all_receptions[name].begin()+=k );
					break;
				}
			}
			if ( !reception_found ) {
				RECEPTION_EVENT r_event;
				r_event.vehicle_name = name;
				r_event.receive_status = 2;
				sync_losses++;
				if ( name == t_event.transmitter_name )
					sync_losses--;
				t_event.receptions_map[name] = r_event;
				t_event.receptions_vector.push_back( r_event );
			}
		}
		all_transmissions[i] = t_event;
//		string tmp;
//		getline( cin, tmp);

	}

	cout << all_transmissions.size() << " transmissions" << endl;
	cout << sync_losses << " sync losses" << endl;
	sum = 0;
	for ( int i=0; i<vehicle_names.size(); i++ ) {
		sum+=all_receptions[vehicle_names[i]].size();
	}
	cout << sum << " unsynced receptions" << endl;

//	generateHistories();


//	cout << endl << endl;
//	parseAllHeaders();
//	parseMOOSFiles();
//
//	cout << endl;
//	generateHistories();
//
//	cout << endl << endl;
//	lookForEvents();
//	cout << endl << "Found " << initial_transmit_events.size() << " transmit events and " <<
//			initial_receive_events.size() << " receive events." << endl;
//
//	sync_losses = 0;
//	self_receives =0;
//	matchWithTime();
//
//	cout << endl << endl;
//	cout << leftover_receive_events.size() << " homeless receipts" << endl;
//	cout << sync_losses << " sync losses and " << self_receives << " self receipts" << endl;

	outputResults();
}
//
//bool history_sort ( ACOMMS_ALOG_PARSER::FILE_INFO f1, ACOMMS_ALOG_PARSER::FILE_INFO f2 ) {
//	return f1.getMOOSTimeOffset() < f2.getMOOSTimeOffset();
//}

//void ACOMMS_ALOG_PARSER::generateHistories() {
//	for ( int i=0; i<alog_files.size(); i++ ) {
////		cout << alog_files[i].vehicle_name << endl;
//		vehicle_histories[alog_files[i].getVehicleName()].vehicle_logs.push_back(alog_files[i]);
//		if ( find( vehicle_names.begin(), vehicle_names.end(), alog_files[i].getVehicleName() ) == vehicle_names.end() ) {
//			vehicle_names.push_back( alog_files[i].getVehicleName() );
//		}
//	}
//	for ( int i=0; i<vehicle_names.size(); i++ ) {
//		string vehicle_name = vehicle_names[i];
//		cout << endl << vehicle_name << endl;
//		sort( 	vehicle_histories[vehicle_name].vehicle_logs.begin(),
//				vehicle_histories[vehicle_name].vehicle_logs.end(),
//				history_sort );
//		for ( int j=0; j<vehicle_histories[vehicle_name].vehicle_logs.size(); j++ ) {
//			cout << vehicle_histories[vehicle_name].vehicle_logs[j].getFilename() << endl;
//		}
//	}
//}

// ------------------------------------------------------------------------------------------------

void ACOMMS_ALOG_PARSER::FILE_INFO::processFile() {
	cout << filename << endl;

//	cout << "opening " << filename << endl;
	openFile();

//	cout << "parsing moos file" << endl;
	parseMOOSFile();
//	cout << "vehicle: " << vehicle_name << endl;
//	cout << "getting header lines" << endl;
	parseHeaderLines();
//	cout << "collecting data" << endl;
	collectData();

//	cout << "closing file" << endl;
	closeFile();

	if ( !offsetViaGPS() ) {
		cout << "Failed to offset using gps, using system time from header." << endl;
		offsetViaHeader();
	}

	cout << "vehicle: " << vehicle_name << "  transmissions: " << transmissions.size() << "  receptions: " << receptions.size() << endl << endl;
}

void ACOMMS_ALOG_PARSER::FILE_INFO::collectData() {
//	cout << "fetching first" << endl;
	ALogEntry entry = getNextEntry();
//	cout << "got first" << endl;
	int iteration = 0;
	while ( entry.getStatus() != "eof" ) {
//		iteration++;
//		if ( iteration%1000 == 0 ) {
//			cout << entry.getTimeStamp() << endl;
//		}
		if ( entry.getStatus() != "invalid" ) {
			string key = entry.getVarName();
			double msg_time = entry.getTimeStamp();
			if ( key == "GPS_X" || key == "NAV_X" ) {
				gps_x.push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( key == "GPS_Y" || key == "NAV_X=Y" ) {
				gps_y.push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( key == "GPS_PTIME" ) {
				try {
					gps_time.push_back( pair<double,ptime>( msg_time,
							time_from_string( entry.getStringVal() ) ) );
				} catch ( exception & e ) {
//					cout << "failed to parse gps_ptime: " << entry.getStringVal() << endl;
				}
			} else if ( key == "DESIRED_THRUST" ) {
				desired_thrust.push_back( pair<double,double>( msg_time, entry.getDoubleVal() ) );
			} else if ( key == "ACOMMS_DRIVER_STATUS" ) {
				acomms_driver_status.push_back( pair<double,string>( msg_time, entry.getStringVal() ) );
			} else if ( key == "ACOMMS_TRANSMIT_DATA" ) {
				acomms_transmit_data.push_back( pair<double,string>( msg_time, entry.getStringVal() ) );
			} else if ( key == "ACOMMS_RECEIVED_ALL" ) {
				receptions.push_back( pair<double, RECEPTION_EVENT>( msg_time,
						constructReception( msg_time, entry.getStringVal() ) ) );
			} else if ( key == "ACOMMS_TRANSMIT_SIMPLE" ) {
				transmissions.push_back( pair<double, TRANSMISSION_EVENT>( msg_time,
						constructTransmission( msg_time, entry.getStringVal() ) ) );
			}
		}

		entry = getNextEntry();
	}
}

ACOMMS_ALOG_PARSER::RECEPTION_EVENT ACOMMS_ALOG_PARSER::FILE_INFO::constructReception( double msg_time, string msg_val ) {
//	cout << msg_time << endl;
//	cout << msg_val << endl;
	RECEPTION_EVENT r_event;
	r_event.vehicle_name = vehicle_name;
	r_event.vehicle_id = vehicle_id;
	r_event.receive_time = msg_time;

	while ( msg_val.find("<|>") != string::npos ) {
		msg_val.replace( msg_val.find("<|>"), 3, "\n" );
	}
	string const* ptr = &msg_val;
	google::protobuf::TextFormat::ParseFromString( *ptr, &r_event.data_msg );

	if ( gps_x.size() > 0 ) {
		r_event.gps_x = gps_x.back().second;
		r_event.gps_y = gps_y.back().second;
		r_event.gps_age = r_event.receive_time-gps_x.back().first;
	}

	for ( int i=acomms_driver_status.size()-1; i>=0; i-- ) {
		if ( acomms_driver_status[i].second == "receiving" ) {
			r_event.receive_start_time = acomms_driver_status[i].first;
			break;
		}
	}

//	cout << "parsed okay" << endl;
	return r_event;
}

ACOMMS_ALOG_PARSER::TRANSMISSION_EVENT ACOMMS_ALOG_PARSER::FILE_INFO::constructTransmission( double msg_time, string msg_val ) {
//	cout << msg_time << endl;
//	cout << msg_val << endl;
	TRANSMISSION_EVENT t_event;
	t_event.transmitter_name = vehicle_name;
	t_event.source_id = vehicle_id;
	t_event.transmission_time = msg_time;
	if ( gps_x.size() > 0 ) {
		t_event.gps_x = gps_x.back().second;
		t_event.gps_y = gps_y.back().second;
		t_event.gps_age = t_event.transmission_time - gps_x.back().first;
	}

	if ( acomms_transmit_data.size()!=0 )
		t_event.data = acomms_transmit_data.back().second;

	SIMPLIFIED_TRANSMIT_INFO ats( msg_val );
	t_event.rate = ats.rate;
	t_event.destination_id = ats.dest;

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
	string moosfilename = filename;
	int extension_index = moosfilename.find(".alog");
	moosfilename.replace(extension_index, 5, "._moos");

	FILE * this_moos_file = fopen( moosfilename.c_str(), "r");
	string line = getNextRawLine( this_moos_file );
	while ( line != "eof" ) {
		// needs improvement
		if ( (line.find("community") != string::npos || line.find("Community") != string::npos)
				 && (line.find("//") == string::npos || line.find("//") > 10 ) ) {
			int tmp_index = line.find("=")+1;
			vehicle_name = stripBlankEnds( line.substr(tmp_index, line.size()-tmp_index) );
			break;
		}
		line = getNextRawLine( this_moos_file );
	}

	fclose( this_moos_file );

	CProcessConfigReader missionreader;
	missionreader.SetFile( moosfilename );
	missionreader.GetConfigurationParam("iacomms_driver", "ID", vehicle_id);
}

void ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaHeader() {
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
	moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0;// - entry.time();
}

bool ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaGPS() {
	if ( gps_time.size() < 2 ) {
		return false;
	} else {
		ptime first_gps_ptime = gps_time[1].second;
		time_duration utc_correction = hours(UTC_TIME_OFFSET);
		first_gps_ptime += utc_correction;
		int first_gps_moos_time_seconds = floor( gps_time[1].first );
		int first_gps_moos_time_milliseconds = floor( gps_time[1].first*1000.0) - first_gps_moos_time_seconds*1000;
		time_duration first_gps_td = seconds ( first_gps_moos_time_seconds ) +
				milliseconds( first_gps_moos_time_milliseconds );

//			cout << "first gps time " << first_gps_td << endl;
//			cout << "ptime " << first_gps_ptime << endl;

		creation_time = first_gps_ptime - first_gps_td;// - first_moos_time_td;
		moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0;// -first_moos_time;
		return true;
	}
}

// ------------------------------------------------------------------------------------------------

void ACOMMS_ALOG_PARSER::addAlogFile( std::string filename ) {
	alog_files.push_back( FILE_INFO( filename ) );
}

void ACOMMS_ALOG_PARSER::addAlogFile( boost::filesystem::path filepath ) {
//	NEVER LOOK HERE
	alog_files.push_back( FILE_INFO( string( filepath.string().c_str() ) ) );
}




//
//
//void ACOMMS_ALOG_PARSER::matchWithTime() {
//	for ( int i=0; i<initial_transmit_events.size(); i++ ) {
//		TRANSMISSION_EVENT t_event = initial_transmit_events[i];
//		double transmit_time = t_event.transmission_time;
//		for ( int j=0; j<vehicle_names.size(); j++ ) {
//			string name = vehicle_names[j];
//			bool receive_found = false;
//			for ( int k=0; k<initial_receive_events.size(); k++ ) {
//				double time_diff = initial_receive_events[k].receive_time - transmit_time;
//				if ( time_diff > 0 && time_diff < TRANSMISSION_TIMEOUT ){
//					t_event.receptions_vector.push_back( initial_receive_events[k] );
//					t_event.receptions_map[name] = initial_receive_events[k];
//					initial_receive_events.erase( initial_receive_events.begin()+=k );
//					receive_found = true;
//
//					if ( initial_receive_events[k].vehicle_name == t_event.transmitter_name )
//						self_receives++;
//
//					break;
//				}
//			}
//			if ( !receive_found ) {
//				if ( name != t_event.transmitter_name )
//					sync_losses++;
//				RECEPTION_EVENT r_event;
//				r_event.receive_status = 2;
//				r_event.vehicle_name = name;
//			}
//		}
//	}
//
//	leftover_receive_events = initial_receive_events;
//}
//
//void ACOMMS_ALOG_PARSER::lookForEvents() {
//	// variables we want to keep an eye on;
//	int gps_x, gps_y;
//	double gps_update_time, receiving_start_time;
//	string acomms_transmit_data;
//
//	// iterate over all vehicles
//	for ( int i=0; i<vehicle_names.size(); i++ ) {
//
//		// get history for current vehicle
//		string my_name = vehicle_names[i];
//		VEHICLE_HISTORY * my_history = &vehicle_histories[my_name];
//
//		// iterate over all log files for that vehicle
//		for ( int j=0; j<my_history->vehicle_logs.size(); j++ ) {
//
//			// get current log file
//			FILE_INFO * my_file = &(my_history->vehicle_logs[j]);
//			cout << "Searching " << my_file->filename << endl;
//			cout << " belonging to " << my_name << endl;
//
//			int rcount = 0, tcount = 0;
//
////			my_file->logfile
//			// look through the file
////			ALogEntry entry = getNextRawALogEntry( my_file->logfile );
//			cout << "getting first line" << endl;
//			cout << my_file->getNextLine() << endl;
//			ALogEntry entry = my_file->getTimeAdjustedNextEntry();
//			cout << "got first entry" << endl;
//			int iteration = 0;
//			while ( entry.getStatus() != "eof" ) {
//				iteration++;
//				if ( iteration%10000 == 0 ) {
//					cout << "MOOSTime: " << entry.getTimeStamp()-my_file->moos_time_offset << endl;
//				}
////				cout << my_file->filename << "  " << entry.getTimeStamp() << endl;
//
//				string key = entry.getVarName();
//				if ( key == "GPS_X" || key == "NAV_X" ) {
//					gps_x = entry.getDoubleVal();
//					gps_update_time = entry.getTimeStamp();
//				} else if ( key == "GPS_Y" || key == "NAV_Y" ) {
//					gps_y = entry.getDoubleVal();
//					gps_update_time = entry.getTimeStamp();
//				} else if ( key == "ACOMMS_TRANSMIT_DATA" ) {
//					acomms_transmit_data = entry.getStringVal();
//				} else if ( key == "ACOMMS_DRIVER_STATUS" ) {
//					if ( entry.getStringVal() == "receiving" ) {
//						receiving_start_time = entry.getTimeStamp();
//					}
//				}
//
//				else if ( key == "ACOMMS_TRANSMIT_SIMPLE" ) {
//					TRANSMISSION_EVENT t_event;
//					t_event.gps_x = gps_x;
//					t_event.gps_y = gps_y;
//					t_event.transmission_time = entry.getTimeStamp();
//					t_event.gps_age = t_event.transmission_time - gps_update_time;
//					t_event.data = acomms_transmit_data;
//					t_event.transmitter_name = my_name;
//
//					SIMPLIFIED_TRANSMIT_INFO ats ( entry.getStringVal() );
//					t_event.rate = ats.rate;
//					t_event.destination_id = ats.dest;
//					t_event.source_id = my_file->vehicle_id;
//
//					initial_transmit_events.push_back( t_event );
//					tcount++;
//				}
//
//				else if ( key == "ACOMMS_RECEIVED_ALL" ) {
//					RECEPTION_EVENT r_event;
//					r_event.gps_x = gps_x;
//					r_event.gps_y = gps_y;
//					r_event.receive_time = entry.getTimeStamp();
//					r_event.gps_age = r_event.receive_time - gps_update_time;
//					r_event.receive_start_time = receiving_start_time;
//					r_event.vehicle_name = my_name;
//					r_event.vehicle_id = my_file->vehicle_id;
//
//					string msg = entry.getStringVal();
//					while ( msg.find("<|>") != string::npos ) {
//						msg.replace( msg.find("<|>"), 3, "\n" );
//					}
//					string const* ptr = &msg;
//					google::protobuf::TextFormat::ParseFromString( *ptr, &r_event.data_msg );
//
////					cout << r_event.data_msg.DebugString() << endl;
////					cout << r_event.receive_time << endl;
//
//					initial_receive_events.push_back( r_event );
//					rcount++;
//				}
//
//				entry = my_file->getTimeAdjustedNextEntry();
//			}
//
//			cout << tcount << " transmissions, " << rcount << " receipts." << endl;
//		}
//	}
//}
//
//void ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaHeader() {
//	int date_start = header_lines[1].find("ON ") + 3;
//	string date_string = header_lines[1].substr(date_start, header_lines[1].size()-date_start);
//	if ( date_string[0] == ' ' )
//		date_string.erase(date_string.begin());
////	cout << date_string << endl;
//	tm header_time;
//	strptime( date_string.c_str(), "%a %b %d %H:%M:%S %Y", &header_time);
////	cout << asctime( &header_time ) << endl;
//	creation_time = boost::posix_time::ptime_from_tm( header_time );
////	cout << boost::posix_time::to_simple_string( creation_time ) << endl;
//
////	ALogEntry entry = getNextRawALogEntry( logfile );
//	moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0;// - entry.time();
//}
//
//bool ACOMMS_ALOG_PARSER::FILE_INFO::offsetViaGPS() {
//	ALogEntry entry = getNextEntry();
//	if ( entry.getStatus() == "invalid" )
//		return false;
////	double first_moos_time = entry.getTimeStamp();
////	int first_moos_time_seconds = floor(entry.getTimeStamp());
////	int first_moos_time_milliseconds = floor(entry.getTimeStamp()*1000.0) - first_moos_time_seconds*1000;
////	time_duration first_moos_time_td = seconds( first_moos_time_seconds ) +
////			milliseconds( first_moos_time_milliseconds );
////
////	cout << "first moos time " << first_moos_time_td << endl;
//	while ( entry.getStatus() != "eof" ) {
//		if ( entry.getVarName() == "GPS_PTIME" ) {
////			cout << ">>" << entry.getStringVal()<< "<<" << endl;
//			ptime first_gps_ptime;
//			try {
//				first_gps_ptime = time_from_string( entry.getStringVal() );
//			} catch ( exception &e ) {
//				return false;
//			}
//			time_duration utc_correction = hours(UTC_TIME_OFFSET);
//			first_gps_ptime += utc_correction;
//			int first_gps_moos_time_seconds = floor(entry.getTimeStamp() );
//			int first_gps_moos_time_milliseconds = floor(entry.getTimeStamp()*1000.0) - first_gps_moos_time_seconds*1000;
//			time_duration first_gps_td = seconds ( first_gps_moos_time_seconds ) +
//					milliseconds( first_gps_moos_time_milliseconds );
//
////			cout << "first gps time " << first_gps_td << endl;
////			cout << "ptime " << first_gps_ptime << endl;
//
//			creation_time = first_gps_ptime - first_gps_td;// - first_moos_time_td;
//			moos_time_offset = creation_time.time_of_day().total_milliseconds()/1000.0;// -first_moos_time;
//			return true;
//		}
//		entry = getNextEntry();
//	}
//	return false;
//}
//
//void ACOMMS_ALOG_PARSER::parseAllHeaders() {
//	for ( int i=0; i<alog_files.size(); i++ ) {
////		alog_files[i].parseHeaderLines();
//		alog_files[i].resetFile();
//		if ( !alog_files[i].offsetViaGPS() ) {
//			cout << "Failed to offset using gps time, using header: " <<
//					alog_files[i].filename << endl;
//			alog_files[i].offsetViaHeader();
//		}
////		cout << alog_files[i].filename << "  moos time offset: " << alog_files[i].moos_time_offset <<
////				"  creation time: " << to_simple_string(alog_files[i].creation_time) << endl;
//	}
//}
//
//void ACOMMS_ALOG_PARSER::parseMOOSFiles() {
//	for ( int i=0; i<alog_files.size(); i++ ) {
//		alog_files[i].parseMOOSFile();
//	}
//}
//
//bool history_sort ( ACOMMS_ALOG_PARSER::FILE_INFO f1, ACOMMS_ALOG_PARSER::FILE_INFO f2 ) {
//	return f1.moos_time_offset < f2.moos_time_offset;
//}
//
//void ACOMMS_ALOG_PARSER::generateHistories() {
//	for ( int i=0; i<alog_files.size(); i++ ) {
////		cout << alog_files[i].vehicle_name << endl;
//		vehicle_histories[alog_files[i].vehicle_name].vehicle_logs.push_back(alog_files[i]);
//		alog_files[i].resetFile();
//		if ( find( vehicle_names.begin(), vehicle_names.end(), alog_files[i].vehicle_name ) == vehicle_names.end() ) {
//			vehicle_names.push_back( alog_files[i].vehicle_name );
//		}
//	}
//	for ( int i=0; i<vehicle_names.size(); i++ ) {
//		string vehicle_name = vehicle_names[i];
//		cout << endl << vehicle_name << endl;
//		sort( 	vehicle_histories[vehicle_name].vehicle_logs.begin(),
//				vehicle_histories[vehicle_name].vehicle_logs.end(),
//				history_sort );
//		for ( int j=0; j<vehicle_histories[vehicle_name].vehicle_logs.size(); j++ ) {
//			cout << vehicle_histories[vehicle_name].vehicle_logs[j].filename << endl;
//		}
//	}
//}
//
////void ACOMMS_ALOG_PARSER::FILE_INFO::resetFile() {
////	if ( file_is_open ) {
////		closeFile();
////	}
////	openFile();
////}
//
////ALogEntry ACOMMS_ALOG_PARSER::FILE_INFO::getTimeAdjustedNextEntry() {
////	ALogEntry next_entry = getNextEntry();
////	next_entry.skewForward( moos_time_offset );
////	return next_entry;
////}

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
