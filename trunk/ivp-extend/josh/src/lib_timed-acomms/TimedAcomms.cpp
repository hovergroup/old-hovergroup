/*
 * TimedAcomms.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: josh
 */

#include "TimedAcomms.h"

TimedAcomms::TimedAcomms() {
	m_ClockSamples = 0;
	m_ExitingTimeCalIterations = 0;
	m_State = GPS_TIME_CAL;

	m_BeginModemReceive = false;
	m_BadReceive = false;
	m_GoodReceive = false;

	m_LastTransmitSlot = -1;
	m_LastReceiveSlot = -1;
}

void TimedAcomms::doWork() {
	m_ThisRunTime = getAbsTimeSeconds();

	switch ( m_State ) {
	case GPS_TIME_CAL:
		doGpsTimeCalState();
		break;
	case EXITING_TIME_CAL:
		doExitingTimeCalState();
		break;
	case READY:
		doReadyState();
		break;
	}

	m_LastRunTime = m_ThisRunTime;
}

/**
 * check if we have enough gps time samples
 */
void TimedAcomms::doGpsTimeCalState() {
	if ( m_ClockSamples == GPS_TIME_SAMPLES )
		m_State = EXITING_TIME_CAL;
}

/**
 * transition state to make sure both run times are synced to gps
 */
void TimedAcomms::doExitingTimeCalState() {
	if ( m_ExitingTimeCalIterations > 2 ) m_State = READY;
	m_ExitingTimeCalIterations++;
}

/**
 * primary ready state from which we enter receptions and transmissions
 */
void TimedAcomms::doReadyState() {
	// check if it is time to transmit
	int transmit_slot = findPreviousSlot( m_LastRunTime, m_TransmitPeriod, m_TransmitOffset );
	if ( transmit_slot > m_LastTransmitSlot ) {
		m_LastTransmitSlot = transmit_slot;
		signal_transmit();
	}

	// check if past receive time
	int receive_slot = findPreviousSlot( m_ThisRunTime, m_ReceivePeriod, m_ReceiveOffset );
	if ( receive_slot > m_LastReceiveSlot ) {
		m_LastReceiveSlot = receive_slot;
		signal_no_receipt();
	}

	// check if modem has started receiving
	if ( m_BeginModemReceive ) {
		// calculate receive slot we expect to fill

		m_BeginModemReceive = false;
		m_State = RECEIVING;
	}
}

/**
 * receiving state where we wait for indication of good/bad receipt
 */
void TimedAcomms::doReceivingState() {
	//

	// check for bad receive signal
	if ( m_BadReceive ) {
		m_BadReceive = false;
		signal_no_receipt();
		m_State = READY;
	}

	// check for good receive signal
	if ( m_GoodReceive ) {
		m_GoodReceive = false;
		signal_receipt( m_ReceivedData );
		m_State = READY;
	}

	// check for timeout
}

void TimedAcomms::signalStartOfModemReceiving() {
	m_BeginModemReceive = true;
}

void TimedAcomms::signalBadReception() {
	m_BadReceive = true;
}

void TimedAcomms::signalGoodReception(std::string data) {
	m_ReceivedData = data;
	m_GoodReceive = true;
}

/**
 * returns seconds from 12am using system clock
 * UTC so should be same as GPS
 */
double TimedAcomms::getSysTimeSeconds() {
	boost::posix_time::ptime t (
			boost::posix_time::microsec_clock::universal_time() );
	return t.time_of_day().total_milliseconds()/1000.0;
}

/**
 * returns seconds from 12am (UTC) using adjusted system clock
 */
double TimedAcomms::getAbsTimeSeconds() {
	return getSysTimeSeconds() + m_ClockOffset;
}

/**
 * uses gps time posting to find system clock offset
 * can stop providing samples when returns false
 */
bool TimedAcomms::processGpsTimeSeconds( double gps_time_seconds ) {
	if ( m_ClockSamples < GPS_TIME_SAMPLES ) {
		double this_error = gps_time_seconds - getSysTimeSeconds();
		m_ClockErrorSum += this_error;
		m_ClockSamples++;
		m_ClockOffset = m_ClockErrorSum / m_ClockSamples;
		return true;
	}
	return false;
}

int TimedAcomms::findNextSlot( double current_time, double period, double offset ) {
	return ceil( (current_time - offset) / period );
}

int TimedAcomms::findPreviousSlot( double current_time, double period, double offset ) {
	return floor( (current_time - offset) / period );
}

double TimedAcomms::slot2Time( int slot, double period, double offset ) {
	return slot*period + offset;
}
