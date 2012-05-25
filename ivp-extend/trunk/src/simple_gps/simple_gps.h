#ifndef __SERIAL_H
#define __SERIAL_H


#include "MOOSLib.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class SIMPLE_GPS : public CMOOSApp
{
public:
	SIMPLE_GPS();
	virtual ~SIMPLE_GPS() {};

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

	void open_port( std::string port_name, int baudRate );
	void close_port();

	void writeData( unsigned char *ptr, int length );

private:
	// basic serial port components
	boost::asio::io_service io;
	boost::asio::serial_port port;

	double m_lat, m_lon, m_speed, m_course, m_lat_origin, m_lon_origin;

	boost::thread serial_thread;
	bool stop_requested;

	std::string my_port_name;
	int my_baud_rate;

	std::string string_buffer;

	// for asyncronous serial port operations
	boost::asio::deadline_timer timeout;
	bool data_available;
	int asyncBytesRead;
	// ----
	void read_handler(bool& data_available, boost::asio::deadline_timer& timeout,
				const boost::system::error_code& error, std::size_t bytes_transferred);
	void wait_callback(boost::asio::serial_port& ser_port, const boost::system::error_code& error);
	void null_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {};

	std::vector<unsigned char> readBuffer, writeBuffer;
	boost::mutex writeBufferMutex;
	int bytesToWrite;

	void parseGPRMC( std::string msg );
	std::vector<std::string> tokenizeString( std::string message, std::string tokens );
	void parseLine( std::string msg );

	// the background loop responsible for interacting with the serial port
	void serialLoop();
	void processWriteBuffer();
};

#endif
