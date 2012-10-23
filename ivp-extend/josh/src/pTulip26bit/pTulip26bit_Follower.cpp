/*
 * pTulip26bit_Follower.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: josh
 */

#include "Tulip26bit.h"

void Tulip26bit::onTransmit_follower() {
	unsigned char transmit_x = LinearEncode( m_osx, m_osx_minimum, m_osx_maximum, 5);
	unsigned char transmit_y = LinearEncode( m_osy, m_osy_minimum, m_osy_maximum, 5);
	unsigned char range = 1;

	std::vector<unsigned char> data (2, 0);
	data[1] = transmit_y<<3 + range;
	data[0] = transmit_x;

	m_Comms.Notify("ACOMMS_TRANSMIT_DATA_BINARY", &data[0], 2);
}

void Tulip26bit::onGoodReceive_follower( const std::string data ) {
	if ( data.size() != 2 ) {
		std::stringstream ss;
		ss << "Wrong data length: " << data.size();
		handleDebug( ss.str() );
	}

	double received_x = LinearDecode( data[0], m_osx_minimum, m_osx_maximum, 5);
	double received_y = LinearDecode( data[1]>>3, m_osy_minimum, m_osy_maximum, 5);

	m_Comms.Notify("COMMANDED_X", received_x);
	m_Comms.Notify("COMMANDED_Y", received_y);
}

void Tulip26bit::onBadReceive_follower() {

}
