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

    m_StateNames[GPS_TIME_CAL] = "GPS_TIME_CAL";
    m_StateNames[EXITING_TIME_CAL] = "EXITING_TIME_CAL";
    m_StateNames[READY] = "READY";
    m_StateNames[RECEIVING] = "RECEIVING";
}

void TimedAcomms::doWork(double moos_time) {
    m_ThisRunTime = moos_time + m_ClockOffset;

    switch (m_State) {
    case GPS_TIME_CAL:
        doGpsTimeCalState();
        break;
    case EXITING_TIME_CAL:
        doExitingTimeCalState();
        break;
    case READY:
        doReadyState();
        break;
    case RECEIVING:
        doReceivingState();
        break;
    }

    m_LastRunTime = m_ThisRunTime;
}

/**
 * check if we have enough gps time samples
 */
void TimedAcomms::doGpsTimeCalState() {
    if (m_ClockSamples == GPS_TIME_SAMPLES)
        m_State = EXITING_TIME_CAL;
}

/**
 * transition state to make sure both run times are synced to gps
 */
void TimedAcomms::doExitingTimeCalState() {
    if (m_ExitingTimeCalIterations > 2) {
        m_State = READY;
        std::stringstream ss;
        ss << "Clock offset is " << m_ClockOffset;
        signal_updates(ss.str());
    }
    m_ExitingTimeCalIterations++;
}

/**
 * primary ready state from which we enter receptions and transmissions
 */
void TimedAcomms::doReadyState() {
    // check if it is time to transmit
    int transmit_slot = findPreviousSlot(m_LastRunTime, m_TransmitPeriod,
            m_TransmitOffset);
    if (transmit_slot > m_LastTransmitSlot) {
        std::stringstream ss;
        ss << "Beginning transmission for slot " << transmit_slot;
        signal_updates(ss.str());

        m_LastTransmitSlot = transmit_slot;
        signal_transmit();
    }

    // check if past receive time
    int receive_slot = findPreviousSlot(m_ThisRunTime, m_ReceivePeriod,
            m_ReceiveOffset);
    if (receive_slot > m_LastReceiveSlot) {
        std::stringstream ss;
        ss << "Notifying no reception for slot " << receive_slot;
        signal_updates(ss.str());

        m_LastReceiveSlot = receive_slot;
        signal_no_receipt();
    }

    // check if modem has started receiving
    if (m_BeginModemReceive) {
        signal_updates("Entering receiving state");

        m_BeginModemReceive = false;
        m_State = RECEIVING;
    }
}

/**
 * receiving state where we wait for indication of good/bad receipt
 */
void TimedAcomms::doReceivingState() {
	// check for good receive signal
	if (m_GoodReceive) {
		// clear flag
		m_GoodReceive = false;
		std::stringstream ss;

		// find nearest matching receive slot
		int slot = findNearestSlot(m_ThisRunTime, m_ReceivePeriod,
				m_ReceiveOffset);

		// check if this is the last slot we used
		if (slot == m_LastReceiveSlot) {
			ss << "Got good receipt for receive slot " << slot
					<< ", but slot already taken.";
			signal_updates(ss.str());
		} else {
			// calculate time and error for slot
			double expected_time = slot2Time(slot, m_ReceivePeriod,
					m_ReceiveOffset);
			double error = fabs(expected_time - m_ThisRunTime);

			// check if error within bounds
			if (error < m_MaxReceivingError) {
				ss << "Got good receipt for receive slot " << slot;
				signal_updates(ss.str());
				signal_receipt(m_ReceivedData);
			} else {
				ss << "Got good receipt for receive slot " << slot
						<< ", but error was out of bounds (" << error << ")";
				signal_updates(ss.str());
			}
		}

		m_State = READY;
		m_LastReceiveSlot = slot;

		return;
	}

	// check for bad receive signal
	if (m_BadReceive) {
		// clear flag
		m_BadReceive = false;
		std::stringstream ss;

		// find nearest matching receive slot
		int slot = findNearestSlot(m_ThisRunTime, m_ReceivePeriod,
				m_ReceiveOffset);

		// check if this is the last slot we used
		if (slot == m_LastReceiveSlot) {
			ss << "Got bad receipt for receive slot " << slot
					<< ", but slot already taken.";
			signal_updates(ss.str());
		} else {
			// calculate time and error for slot
			double expected_time = slot2Time(slot, m_ReceivePeriod,
					m_ReceiveOffset);
			double error = fabs(expected_time - m_ThisRunTime);

			// check if error within bounds
			if (error < m_MaxReceivingError) {
				ss << "Got bad receipt for receive slot " << slot;
				signal_updates(ss.str());
				signal_no_receipt();
			} else {
				ss << "Got bad receipt for receive slot " << slot
						<< ", but error was out of bounds (" << error << ")";
				signal_updates(ss.str());
			}
		}

		// return to ready state and update slot
		m_State = READY;
		m_LastReceiveSlot = slot;

		return;
	}

	// check for timeout
	int slot = findNearestSlot(m_ThisRunTime, m_ReceivePeriod, m_ReceiveOffset);
	double expected_time = slot2Time(slot, m_ReceivePeriod, m_ReceiveOffset);
	if ( m_ThisRunTime - expected_time > m_AllowedReceivingExtension ) {
		std::stringstream ss;
		ss << "Timed out in receive slot " << slot;
		signal_updates(ss.str());

		signal_no_receipt();
		m_State = READY;
		m_LastReceiveSlot = slot;
	}
}

void TimedAcomms::signalStartOfModemReceiving() {
    m_BeginModemReceive = true;
}

void TimedAcomms::signalBadReception() {
    if (m_State == RECEIVING) {
        m_BadReceive = true;
    } else {
        signal_debug(
                "WARNING - tried to signal bad receipt during state "
                        + m_StateNames[m_State]);
    }
}

void TimedAcomms::signalGoodReception(std::string data) {
    if (m_State == RECEIVING) {
        m_ReceivedData = data;
        m_GoodReceive = true;
    } else {
        signal_debug(
                "WARNING - tried to signal good reception during state "
                        + m_StateNames[m_State]);
    }
}

/**
 * returns seconds from 12am using system clock
 * UTC so should be same as GPS
 */
//double TimedAcomms::getSysTimeSeconds() {
//    boost::posix_time::ptime t(
//            boost::posix_time::microsec_clock::universal_time());
//    return t.time_of_day().total_milliseconds() / 1000.0;
//}

/**
 * returns seconds from 12am (UTC) using adjusted system clock
 */
//double TimedAcomms::getAbsTimeSeconds() {
//    return getSysTimeSeconds() + m_ClockOffset;
//}

/**
 * uses gps time posting to find system clock offset
 * can stop providing samples when returns false
 */
bool TimedAcomms::processGpsTimeSeconds(double gps_time_seconds, double moos_time) {
    if (m_ClockSamples < GPS_TIME_SAMPLES) {
        double this_error = gps_time_seconds - moos_time;
        m_ClockErrorSum += this_error;
        m_ClockSamples++;
        m_ClockOffset = m_ClockErrorSum / m_ClockSamples;
        return true;
    }
    return false;
}

int TimedAcomms::findNextSlot(double current_time, double period,
        double offset) {
    return ceil((current_time - offset) / period);
}

int TimedAcomms::findPreviousSlot(double current_time, double period,
        double offset) {
    return floor((current_time - offset) / period);
}

int TimedAcomms::findNearestSlot(double current_time, double period,
        double offset) {
    return ceil((current_time - offset) / period - .5);
}

double TimedAcomms::slot2Time(int slot, double period, double offset) {
    return slot * period + offset;
}

void TimedAcomms::setReceiveTiming(double period, double offset) {
    m_ReceivePeriod = period;
    m_ReceiveOffset = offset;
}

void TimedAcomms::setTransmitTiming(double period, double offset) {
    m_TransmitPeriod = period;
    m_TransmitOffset = offset;
}

void TimedAcomms::setReceivingExtension(double extension) {
    m_AllowedReceivingExtension = extension;
}

void TimedAcomms::setMaxReceivingError(double error) {
	m_MaxReceivingError = error;
}