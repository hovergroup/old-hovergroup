#include "serial.h"
#include "parser.h"

using namespace std;
using namespace boost::asio;

SERIAL_VANILLA::SERIAL_VANILLA() : port(io), timeout(io) {
	// initialize buffers with some size larger than should ever be needed
	writeBuffer = vector<unsigned char> (1000, 0);
	readBuffer = vector<unsigned char> (1000, 0);
	
	bytesToWrite = 0;
	asyncBytesRead = 0;
	data_available = false;
	stop_requested = false;
}

void SERIAL_VANILLA::open_port( string port_name, int baudRate ) {
	// open the serial port
	port.open(port_name);
	
	// serial port must be configured after being opened
	port.set_option(serial_port_base::baud_rate(baudRate));
	port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
	port.set_option(serial_port_base::parity(serial_port_base::parity::none));
	port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
	port.set_option(serial_port_base::character_size(8));
	
	// start the background thread
	serial_thread = boost::thread(boost::bind(&SERIAL_VANILLA::serialLoop, this));
}

void SERIAL_VANILLA::close_port() {
	stop_requested = true;
	serial_thread.join();
	port.close();
}

void SERIAL_VANILLA::writeData( unsigned char *ptr, int length ) {
	writeBufferMutex.lock();
	memcpy(&writeBuffer[bytesToWrite], ptr, length);
	bytesToWrite += length;
	writeBufferMutex.unlock();
}

bool SERIAL_VANILLA::active(){
	return stop_requested;
}

void SERIAL_VANILLA::read_handler(bool& data_available, deadline_timer& timeout, 
	const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (error || !bytes_transferred) {
		// no data read
		data_available = false;
		return;
	}
	
	data_available = true;
	asyncBytesRead = bytes_transferred;
	timeout.cancel();
}

void SERIAL_VANILLA::wait_callback(serial_port& ser_port, const boost::system::error_code& error)
{
	if (error) {
		// data read, timeout cancelled
		return;
	}
	port.cancel(); // read_callback fires with error
}

void SERIAL_VANILLA::processWriteBuffer() {
	// take out lock
	writeBufferMutex.lock();
	if ( bytesToWrite > 0 ) {
		 
		// if there is data waiting, copy it to a local buffer
		vector<unsigned char> localWriteBuffer( bytesToWrite, 0 );
		memcpy(&localWriteBuffer[0], &writeBuffer[0], bytesToWrite);
		bytesToWrite = 0;
		// release lock to prevent outside write requests from blocking on serial write
		writeBufferMutex.unlock();
		
		cout << endl << dec << "writing " << localWriteBuffer.size() << " bytes" << endl;
		
		// simple synchronous serial write
		port.write_some( buffer(localWriteBuffer, localWriteBuffer.size()) );
	} else {
		// no data to write, release lock
		writeBufferMutex.unlock();
	}
}

void SERIAL_VANILLA::serialLoop() {
	while (!stop_requested) {
		
		processWriteBuffer();
		
		// set up an asynchronous read that will read up to 100 bytes, but will return as soon as any bytes area read
		// bytes read will be placed into readBuffer starting at index 0
		port.async_read_some( buffer( &readBuffer[0], 100 ), 
				boost::bind( &SERIAL_VANILLA::read_handler, this, boost::ref(data_available), boost::ref(timeout), 
					boost::asio::placeholders::error, 
					boost::asio::placeholders::bytes_transferred ) );
					
		// setup a timer that will prevent the asynchronous operation for more than 100 ms
		timeout.expires_from_now( boost::posix_time::milliseconds(100) );
		timeout.async_wait( boost::bind( &SERIAL_VANILLA::wait_callback, this, boost::ref(port), 
				boost::asio::placeholders::error ) );
		
		// reset then run the io service to start the asynchronous operation
		io.reset();
		io.run();
		
		if (data_available) {
			// print out read data in hexidecimal format
			for (int i=0; i<asyncBytesRead; i++) {
				cout << readBuffer[i];
			}
			cout.flush();
		}
	}
}
