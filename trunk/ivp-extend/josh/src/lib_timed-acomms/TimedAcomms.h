/*
 * TimedAcomms.h
 *
 *  Created on: Oct 15, 2012
 *      Author: josh
 */

#ifndef TIMEDACOMMS_H_
#define TIMEDACOMMS_H_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/signals2.hpp>
#include <map>

class TimedAcomms {
public:
    TimedAcomms();

    void doWork(double moos_time);

    // data inputs
    bool processGpsTimeSeconds(double gps_time_seconds, double moos_time);
    void signalStartOfModemReceiving();
    void signalBadReception();
    void signalGoodReception(std::string data);
    void signalBreakFromReceiving();

    // output signals - use goby methods to connect
    boost::signals2::signal<void()> signal_no_receipt;
    boost::signals2::signal<void(const std::string data)> signal_receipt;
    boost::signals2::signal<void()> signal_transmit;
    boost::signals2::signal<void(const std::string msg)> signal_debug,
            signal_updates;

    // configuration
    void setReceiveTiming(double period, double offset);
    void setTransmitTiming(double period, double offset);
    void setReceivingExtension(double extension);
    void setMaxReceivingError(double error);

private:
    static const int GPS_TIME_SAMPLES = 20;

    enum StateIDs {
        GPS_TIME_CAL = 0, EXITING_TIME_CAL, READY, RECEIVING
    };

    StateIDs m_State;
    std::map<StateIDs, std::string> m_StateNames;

//    double getSysTimeSeconds(); // system clock
//    double getAbsTimeSeconds(); // system clock with gps correction

    // clock adjustment
    double m_ClockOffset;
    double m_ClockErrorSum;
    int m_ClockSamples;

    // timing configuration
    double m_TransmitPeriod, m_TransmitOffset;
    double m_ReceivePeriod, m_ReceiveOffset;
    double m_AllowedReceivingExtension;
    double m_MaxReceivingError;

    // timing operations
    int findNextSlot(double current_time, double period, double offset);
    int findPreviousSlot(double current_time, double period, double offset);
    int findNearestSlot(double current_time, double period, double offset);
    double slot2Time(int slot, double period, double offset);

    // state methods
    void doReadyState();
    void doExitingTimeCalState();
    void doGpsTimeCalState();
    void doReceivingState();

    // transmission
    int m_LastTransmitSlot;

    // receiving
    int m_LastReceiveSlot;

    // persistent data
    int m_ExitingTimeCalIterations;
    double m_ThisRunTime, m_LastRunTime;

    // incoming data
    bool m_BeginModemReceive, m_BadReceive, m_GoodReceive, m_IgnoreReceive;
    std::string m_ReceivedData;
};

#endif /* TIMEDACOMMS_H_ */
