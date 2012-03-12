#ifndef __SERIAL_H
#define __SERIAL_H

// the other includes needed (such as string, vector, etc.) are apparent already in these three
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class SERIAL_VANILLA
{
public:
	SERIAL_VANILLA();

	void open_port( std::string port_name, int baudRate );
	void close_port();
	
	void writeData( unsigned char *ptr, int length );
	
	//Returns service state
	bool active();
	
private:
	// basic serial port components
	boost::asio::io_service io;
	boost::asio::serial_port port;
	
	boost::thread serial_thread;
	bool stop_requested;
	
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
	
	// the background loop responsible for interacting with the serial port
	void serialLoop();
	void processWriteBuffer();
};

#endif 
