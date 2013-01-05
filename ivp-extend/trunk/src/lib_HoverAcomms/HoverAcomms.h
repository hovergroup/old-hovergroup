/*
 * JoshUtil.h
 *
 *  Created on: Nov 11, 2012
 *      Author: josh
 */

#ifndef LIB_HOVERACOMMS_H_
#define LIB_HOVERACOMMS_H_

#include "goby/acomms/modem_driver.h"
#include "goby/acomms/protobuf/mm_driver.pb.h"
#include <string>
#include <map>
#include "boost/assign.hpp"

namespace HoverAcomms {

enum Rate {
	FSK0 = 0,
	PSK1,
	PSK2,
	PSK3,
	PSK4,
	PSK5,
	PSK6,
	MINI
};

enum ReceptStatus {
	GOOD = 0, // all frames received good
	PARTIAL, // some frames received good, some bad
	BAD // no frames received
};

static const std::map<int,Rate> ReverseRateMap = boost::assign::map_list_of
		(0,FSK0)
		(1,PSK1)
		(2,PSK2)
		(3,PSK3)
		(4,PSK4)
		(5,PSK5)
		(6,PSK6);

static const std::map<Rate,int> FrameSizeMap = boost::assign::map_list_of
		(FSK0,32)
		(PSK1,64)
		(PSK2,64)
		(PSK3,256)
		(PSK4,256)
		(PSK5,256)
		(PSK6,32);
static const std::map<Rate,int> FrameCountMap = boost::assign::map_list_of
		(FSK0,1)
		(PSK1,3)
		(PSK2,3)
		(PSK3,2)
		(PSK4,2)
		(PSK5,8)
		(PSK6,6);

class AcommsReception {
public:
	AcommsReception() {}

	std::string Serialize() { return m_protobuf.SerializeAsString(); }
	bool parseFromString(std::string & msg);
	void copyFromProtobuf(goby::acomms::protobuf::ModemTransmission & proto);
	std::string verify(bool & ok);

	int getSource() const { return m_protobuf.src(); }
	int getDest() const { return m_protobuf.dest(); }
	int getNumFrames() const { return m_protobuf.frame_size(); }
	int getNumStats() const { return m_protobuf.ExtensionSize(micromodem::protobuf::receive_stat); }

	bool hasRanging() const;
	double getRangingTime() const;

	Rate getRate() const;
	std::string getFrame(unsigned int i) const;
	bool frameOkay(unsigned int i) const;
	std::string getAllFrames() const;

	const micromodem::protobuf::ReceiveStatistics & getStatistics(unsigned int i=0) const;

protected:
	goby::acomms::protobuf::ModemTransmission m_protobuf;

	Rate reverseRate(int i) const { return ReverseRateMap.find(i)->second; }

};

class AcommsTransmission {
public:
	AcommsTransmission() {}
	AcommsTransmission(std::string data, Rate rate, int dest=0);

	std::string Serialize() { return m_protobuf.SerializeAsString(); }
	bool parseFromString(std::string msg);

	std::string getLoggableString() const;

	bool setRate(Rate r);
	bool setRate(int r);
	void setDest(int d) { m_protobuf.set_dest(d); }

	int fillData(char * data, int length);
	int fillData(std::string data);
	std::string getHexData() const;

	std::string 	getData() const	{ return m_data; }
	Rate 			getRate() const	{ return m_rate; }
	int				getDest() const { return m_protobuf.dest(); }

	const goby::acomms::protobuf::ModemTransmission & getProtobuf() const {return m_protobuf;}

protected:
	goby::acomms::protobuf::ModemTransmission m_protobuf;
	Rate m_rate;
	std::string m_data;

	void packMessage(std::string data);

	int frameSize() { return FrameSizeMap.find(m_rate)->second; }
	int frameCount() { return FrameCountMap.find(m_rate)->second; }

	Rate reverseRate(int i) const { return ReverseRateMap.find(i)->second; }
};

};

#endif // LIB_HOVERACOMMS_H_
