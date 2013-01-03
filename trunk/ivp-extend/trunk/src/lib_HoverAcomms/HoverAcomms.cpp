/*
 * JoshUtil.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: josh
 */

#include "HoverAcomms.h"

using namespace HoverAcomms;

void AcommsTransmission::setRate(Rate r) {
	m_rate = r;
	if (r==MINI) {
		m_protobuf.set_type(goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC);
		m_protobuf.SetExtension( micromodem::protobuf::type, micromodem::protobuf::MICROMODEM_MINI_DATA );
	} else {
		m_protobuf.set_type(goby::acomms::protobuf::ModemTransmission::DATA);
		m_protobuf.set_rate(r);
	}
}

// pack data into frames
void AcommsTransmission::packMessage(std::string data) {
	m_protobuf.clear_frame();
	m_data.clear();

	int frame_size = frameSize();
	int frame_count = frameCount();

	// just a single frame
	if ( data.size() <= frame_size ) {
		m_protobuf.add_frame(data);
		m_data = data;
	} else { // multiple frames
		int filled_size = 0;
		// pack in full frames
		while ( data.size() > frame_size && m_protobuf.frame_size()<frame_size ) {
			m_protobuf.add_frame(data.data(), frame_size);
			data = data.substr(frame_size, data.size()-frame_size);
		}
		// fill up last frame or use remaining data
		if (data.size()>0 && m_protobuf.frame_size()<frame_count ) {
			int leftover = std::min(frame_size, (int) data.size());
			m_protobuf.add_frame(data.data(), leftover);
		}

		for (int i=0; i<m_protobuf.frame_size(); i++) {
			m_data += m_protobuf.frame(i);
		}
	}
}

int AcommsTransmission::fillData(char * data, int length) {
	return fillData(std::string(data, length));
}

int AcommsTransmission::fillData(std::string data) {
	if (m_rate==MINI) {
		if (data.size()==1) {
			char filler = 0x00;
			data.insert(0,&filler,1);
		}
		m_protobuf.add_frame(data.data(),2);
		m_protobuf.mutable_frame(0)->at(0) &= 0x1f;
		m_data = m_protobuf.frame(0);
		return 2;
	} else {
		packMessage(data);
		return m_data.size();
	}
}

AcommsTransmission::AcommsTransmission(std::string data, Rate rate, int dest) {
	setDest(dest);
	setRate(rate);
	fillData(data);
}

bool AcommsTransmission::parseFromString(std::string & msg) {
	m_protobuf.Clear();
	return m_protobuf.ParseFromString(msg);
}

bool AcommsReception::parseFromString(std::string & msg) {
	m_protobuf.Clear();
	return m_protobuf.ParseFromString(msg);
}

void AcommsReception::copyFromProtobuf(goby::acomms::protobuf::ModemTransmission & proto) {
	m_protobuf.Clear();
	m_protobuf.CopyFrom(proto);
}

std::string  AcommsReception::verify(bool & ok) {
	std::stringstream ss;

	// verify number of statistics against rate
	int num_stats = m_protobuf.ExtensionSize(micromodem::protobuf::receive_stat);
	if (getRate()==FSK0) {
		if (num_stats!=2) {
			ss << "FSK packet had " << num_stats << "receive statistics.";
			ok = false;
			return ss.str();
		}
	} else if (num_stats!=1) {
		ss << "Non-FSK packet had " << num_stats << "receive statistics.";
		ok = false;
		return ss.str();
	}

	micromodem::protobuf::ReceiveStatistics my_stat;
	if (getRate()==FSK0) {
		my_stat = getStatistics(1);
	} else {
		my_stat = getStatistics(0);
	}
	if (my_stat.number_frames() != m_protobuf.frame_size()) {
		ss << "Statistics # frames (" << my_stat.number_frames() <<
				") did not match found frames (" << m_protobuf.frame_size() << ")";
		ok = false;
		return ss.str();
	}
	if (my_stat.number_bad_frames()<0 || my_stat.number_bad_frames()>my_stat.number_frames()) {
		ss << "No. bad frames (" << my_stat.number_bad_frames() <<
				") did not fall within bounds [0 " <<
				my_stat.number_frames() << "]";
		ok = false;
		return ss.str();
	}

	ok = true;
	return "";
}

Rate AcommsReception::getRate() const {
	if (m_protobuf.type() == goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC)
		return MINI;
	else
		return reverseRate(m_protobuf.rate());
}

std::string AcommsReception::getFrame(unsigned int i) const {
	if (i>getNumFrames())
		return "";
	else
		return m_protobuf.frame(i);
}

bool AcommsReception::frameOkay(unsigned int i) const {
	int numbad = m_protobuf.ExtensionSize(micromodem::protobuf::frame_with_bad_crc);
	for (int j=0; j<numbad; j++) {
		if (i == m_protobuf.GetExtension(micromodem::protobuf::frame_with_bad_crc, j))
			return false;
	}
	return true;
}

std::string AcommsReception::getAllFrames() const {
	std::string s;
	for (int i=0; i<getNumFrames(); i++) {
		s+=getFrame(i);
	}
	return s;
}

const micromodem::protobuf::ReceiveStatistics & AcommsReception::getStatistics(unsigned int i) const {
	if (i>=getNumStats()) {
		i=getNumStats()-1;
	}
	return m_protobuf.GetExtension(micromodem::protobuf::receive_stat, i);
}

bool AcommsReception::hasRanging() const {
	// check that extension is there
	if (m_protobuf.HasExtension(micromodem::protobuf::ranging_reply))
		return true;
	else
		return false;
}

double AcommsReception::getRangingTime() const {
	if (!hasRanging()) return -1;

	micromodem::protobuf::RangingReply ranging = m_protobuf.GetExtension(micromodem::protobuf::ranging_reply);
	if (ranging.one_way_travel_time_size()<1) {
		return -1;
	} else {
		return ranging.one_way_travel_time(0);
	}
}
