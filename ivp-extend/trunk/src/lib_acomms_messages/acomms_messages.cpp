/*
 * lib_receive_info.cpp
 *
 *  Created on: May 24, 2012
 *      Author: josh
 */

#include "acomms_messages.h"
#include <string.h>

using namespace lib_acomms_messages;
using namespace std;

string string_chomp( string s, char c ) {
	for ( int i=0; i<s.size()-1; i++ ) {
		if ( s[i]==c ) return s.substr(i+1, s.size()-i+1);
	}
	return s;
}

vector<string> tokenizeString( string message, string tokens ) {
	char * cstr = new char [message.size() + 1];
	strcpy(cstr, message.c_str());
	char * ctokens = new char [tokens.size()+1];
	strcpy(ctokens, tokens.c_str());

	char * pch;
	vector<string> subs;

	pch = strtok(cstr,ctokens);
	while (pch != NULL) {
		subs.push_back(string(pch));
		pch  = strtok(NULL,ctokens);
	}

	delete cstr;
	delete ctokens;

	return subs;
}

SIMPLIFIED_RECEIVE_INFO::SIMPLIFIED_RECEIVE_INFO( string msg ) {
	vector<string> substrings;
	int pos = 0;
	while ( msg.find(":", pos) != string::npos ) {
		int newpos = msg.find(":", pos);
		string temp_sub = msg.substr(pos, newpos-pos);
		int another_pos = temp_sub.find(",");
		substrings.push_back( temp_sub.substr(another_pos+1,temp_sub.size()-another_pos) );
		pos = newpos+1;
	}
	string temp_sub = msg.substr(pos, msg.size()-pos);
	int another_pos = temp_sub.find(",");
	substrings.push_back( temp_sub.substr(another_pos+1,temp_sub.size()-another_pos) );

	if ( substrings.size() >=6 ) {
		vehicle_name = substrings[0];
		source = atoi(substrings[1].c_str());
		rate = atoi(substrings[2].c_str());
		num_frames = atoi(substrings[3].c_str());
		num_good_frames = atoi(substrings[4].c_str());
		num_bad_frames = atoi(substrings[5].c_str());
	}
}

string SIMPLIFIED_RECEIVE_INFO::serializeToString() {
	stringstream ss;

	ss << "vehicle_name," << vehicle_name;
	ss << ":source," << source;
	ss << ":rate," << rate;
	ss << ":num_frames," << num_frames;
	ss << ":num_good_frames," << num_good_frames;
	ss << ":num_bad_frames," << num_bad_frames;

	return ss.str();
}

SIMPLIFIED_TRANSMIT_INFO::SIMPLIFIED_TRANSMIT_INFO( string msg ) {
	vector<string> substrings;
	int pos = 0;
	while ( msg.find(":", pos) != string::npos ) {
		int newpos = msg.find(":", pos);
		string temp_sub = msg.substr(pos, newpos-pos);
		int another_pos = temp_sub.find(",");
		substrings.push_back( temp_sub.substr(another_pos+1,temp_sub.size()-another_pos) );
		pos = newpos+1;
	}
	string temp_sub = msg.substr(pos, msg.size()-pos);
	int another_pos = temp_sub.find(",");
	substrings.push_back( temp_sub.substr(another_pos+1,temp_sub.size()-another_pos) );

	if ( substrings.size() >=4 ) {
		vehicle_name = substrings[0];
		rate = atoi(substrings[1].c_str());
		dest = atoi(substrings[2].c_str());
		num_frames = atoi(substrings[3].c_str());
	}
}

string SIMPLIFIED_TRANSMIT_INFO::serializeToString() {
	stringstream ss;

	ss << "vehicle_name," << vehicle_name;
	ss << ":rate," << rate;
	ss << ":dest," << dest;
	ss << ":num_frames," << num_frames;

	return ss.str();
}

LOSS_RATE_INFO::LOSS_RATE_INFO(string msg){
	vector<string> substrings;
		int pos = 0;
		while ( msg.find(":", pos) != string::npos ) {
			int newpos = msg.find(":", pos);
			string temp_sub = msg.substr(pos, newpos-pos);
			int another_pos = temp_sub.find(",");
			substrings.push_back( temp_sub.substr(another_pos+1,temp_sub.size()-another_pos) );
			pos = newpos+1;
		}
		string temp_sub = msg.substr(pos, msg.size()-pos);
		int another_pos = temp_sub.find(",");
		substrings.push_back( temp_sub.substr(another_pos+1,temp_sub.size()-another_pos) );

		if ( substrings.size() >=5 ) {
			transmitter_name = substrings[0];
			receiver_name = substrings[1];
			sync_loss_rate = atoi(substrings[2].c_str());
			bad_crc_loss_rate = atoi(substrings[3].c_str());
			success_rate = atoi(substrings[4].c_str());
		}
}

LOSS_RATE_INFO::LOSS_RATE_INFO(string transmitter, string receiver, double sync, double crc, double success){
	transmitter_name = transmitter;
	receiver_name = receiver;
	sync_loss_rate = sync;
	bad_crc_loss_rate = crc;
	success_rate = success;
}

string LOSS_RATE_INFO::serializeToString(){

	stringstream ss;
	ss << "transmitter," << transmitter_name;
	ss << ":receiver," << receiver_name;
	ss << ":syncloss," << sync_loss_rate;
	ss << ":crcloss," << bad_crc_loss_rate;
	ss << ":success," << success_rate;

	return ss.str();
}
