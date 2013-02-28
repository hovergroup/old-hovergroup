/*
 * JoshUtil.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: josh
 */

#include "HoverAcomms.h"

using namespace HoverAcomms;

bool AcommsTransmission::setRate(Rate r) {
	m_rate = r;
	if (r==MINI) {
		m_protobuf.set_type(goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC);
		m_protobuf.SetExtension( micromodem::protobuf::type, micromodem::protobuf::MICROMODEM_MINI_DATA );
	} else {
		m_protobuf.set_type(goby::acomms::protobuf::ModemTransmission::DATA);
		m_protobuf.set_rate(r);
	}
	return true;
}

bool AcommsTransmission::setRate(int r) {
	if (r == 100) {
		m_rate = MINI;
		m_protobuf.set_type(goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC);
		m_protobuf.SetExtension( micromodem::protobuf::type, micromodem::protobuf::MICROMODEM_MINI_DATA );
	} else if (r<0 || r>6) {
		return false;
	} else {
		m_rate = reverseRate(r);
		m_protobuf.set_type(goby::acomms::protobuf::ModemTransmission::DATA);
		m_protobuf.set_rate(m_rate);
	}
	return true;
}

// pack data into frames
void AcommsTransmission::packMessage(std::string data) {
	m_protobuf.clear_frame();

	int frame_size = frameSize();
	int frame_count = frameCount();

	// just a single frame
	if ( data.size() <= frame_size ) {
		m_protobuf.add_frame(data);
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
	}
}

int AcommsTransmission::fillData(const char * data, int length) {
	return fillData(std::string(data, length));
}

int AcommsTransmission::fillData(const std::string & data) {
	if (m_rate==MINI) {
		std::string data_copy = data;
		if (data_copy.size()==1) {
			char filler = 0x00;
			data_copy.insert(0,&filler,1);
		}
		m_protobuf.add_frame(data_copy.data(),2);
		m_protobuf.mutable_frame(0)->at(0) &= 0x1f;
		return 2;
	} else {
		packMessage(data);
		return getData().size();
	}
}

std::string AcommsBase::getLoggableString() const {
	std::string publish_me = m_protobuf.DebugString();
	while (publish_me.find("\n") != std::string::npos) {
		publish_me.replace(publish_me.find("\n"), 1, "<|>");
	}
	return publish_me;
}

std::string AcommsBase::getData() const {
	std::string s;
	for (int i=0; i<getNumFrames(); i++) {
		s += m_protobuf.frame(i);
	}
	return s;
}

std::string AcommsBase::getHexData() const {
    std::stringstream ss;
    std::string data = getData();
    for ( int i=0; i<data.size(); i++ ) {
    	ss << std::hex << (int) data[i];
		if ( i < data.size()-1 )
			ss << ":";
    }
    return ss.str();
}

AcommsTransmission::AcommsTransmission(std::string data, Rate rate, int dest) {
	setDest(dest);
	setRate(rate);
	fillData(data);
}

bool AcommsBase::parseFromString(std::string msg) {
	m_protobuf.Clear();
	m_vehicleName = "";
	if (msg.find("vname=") != std::string::npos) {
		MOOSChomp(msg, "vname=");
		m_vehicleName = MOOSChomp(msg, ":");
		MOOSChomp(msg, "time=");
		m_time = atof(MOOSChomp(msg, ":").c_str());
		MOOSChomp(msg, "loc=");
		m_navx = atof(MOOSChomp(msg,",").c_str());
		MOOSChomp(msg, ",");
		m_navy = atof(MOOSChomp(msg,":").c_str());
		MOOSChomp(msg, ":");
	}
	return m_protobuf.ParseFromString(msg);
}

void AcommsBase::copyFromProtobuf(const goby::acomms::protobuf::ModemTransmission & proto) {
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

ReceiptStatus AcommsReception::getStatus() const {
	if (getNumFrames()>0 && getNumBadFrames()<getNumFrames()) {
		return PARTIAL;
	} else if (getNumFrames()>0 && getNumBadFrames()==0) {
		return GOOD;
	} else {
		return BAD;
	}
}

std::string AcommsReception::getBadFrameListing() const {
	std::stringstream ss;
	for (int i=0; i<getNumBadFrames(); i++) {
		ss << m_protobuf.GetExtension(micromodem::protobuf::frame_with_bad_crc,i);
		if (i<getNumBadFrames()-1) ss << ",";
	}
	return ss.str();
}

bool AcommsReception::frameOkay(unsigned int i) const {
	for (int j=0; j<getNumBadFrames(); j++) {
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