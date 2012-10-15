#include <iterator>
#include "simple_gps.h"
#include "MBUtils.h"

using namespace std;
using namespace boost::asio;
using namespace boost::posix_time;

SIMPLE_GPS::SIMPLE_GPS() : port(io), timeout(io) {
	// initialize buffers with some size larger than should ever be needed
	writeBuffer = vector<unsigned char> (1000, 0);
	readBuffer = vector<unsigned char> (1000, 0);

	bytesToWrite = 0;
	asyncBytesRead = 0;
	data_available = false;
	stop_requested = false;

	my_baud_rate = 38400;
	my_port_name = "/dev/ttyUSB0";

	string_buffer = "";
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SIMPLE_GPS::OnNewMail(MOOSMSG_LIST &NewMail)
{
	return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SIMPLE_GPS::OnConnectToServer()
{
	STRING_LIST sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	m_MissionReader.GetConfiguration(GetAppName(), sParams);

	// get lat and long origin from moos file
	double m_lat_origin, m_lon_origin;
	bool ok1, ok2;
	double * ptr = &m_lat_origin;
	ok1 = m_MissionReader.GetValue("LatOrigin", *ptr);
	ptr = &m_lon_origin;
	ok2 = m_MissionReader.GetValue("LongOrigin", *ptr);
	if ( !ok1 || !ok2 ) {
		cout << "Error reading Lat/Long origin from MOOS file." << endl;
		return false;
	}

	// initialize geodesy
	if ( !m_Geodesy.Initialise(m_lat_origin, m_lon_origin) ) {
		cout << "Error initializing geodesy" << endl;
		return false;
	}

	// port name and baud rate
	m_MissionReader.GetConfigurationParam("BAUD_RATE", my_baud_rate);
	m_MissionReader.GetConfigurationParam("PORT_NAME", my_port_name);

	RegisterVariables();

	open_port( my_port_name, my_baud_rate );
	//serial_thread = boost::thread(boost::bind(&SIMPLE_GPS::serialLoop, this));

	return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void SIMPLE_GPS::RegisterVariables()
{
}


//---------------------------------------------------------
// Procedure: Iterate()

bool SIMPLE_GPS::Iterate()
{
	return(true);
}



//---------------------------------------------------------
// Procedure: OnStartUp()
//      Note: happens before connection is open

bool SIMPLE_GPS::OnStartUp()
{
	// I prefer to do nothing here
	return(true);
}

void SIMPLE_GPS::open_port( string port_name, int baudRate ) {
	if ( port.is_open() ) return;
	// open the serial port
	cout << "Opening " << port_name << endl;
	port.open(port_name);

	// serial port must be configured after being opened
	port.set_option(serial_port_base::baud_rate(baudRate));
	port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
	port.set_option(serial_port_base::parity(serial_port_base::parity::none));
	port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
	port.set_option(serial_port_base::character_size(8));

	// start the background thread
	serial_thread = boost::thread(boost::bind(&SIMPLE_GPS::serialLoop, this));
}

void SIMPLE_GPS::close_port() {
	stop_requested = true;
	serial_thread.join();
	port.close();
}

void SIMPLE_GPS::writeData( unsigned char *ptr, int length ) {
	writeBufferMutex.lock();
	memcpy(&writeBuffer[bytesToWrite], ptr, length);
	bytesToWrite += length;
	writeBufferMutex.unlock();
}

void SIMPLE_GPS::read_handler(bool& data_available, deadline_timer& timeout,
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

void SIMPLE_GPS::wait_callback(serial_port& ser_port, const boost::system::error_code& error)
{
	if (error) {
		// data read, timeout cancelled
		return;
	}
	port.cancel(); // read_callback fires with error
}

void SIMPLE_GPS::processWriteBuffer() {
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

void SIMPLE_GPS::serialLoop() {
	while (!stop_requested) {

		processWriteBuffer();

		// set up an asynchronous read that will read up to 100 bytes, but will return as soon as any bytes area read
		// bytes read will be placed into readBuffer starting at index 0
		port.async_read_some( buffer( &readBuffer[0], 1000 ),
				boost::bind( &SIMPLE_GPS::read_handler, this, boost::ref(data_available), boost::ref(timeout),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred ) );
		// setup a timer that will prevent the asynchronous operation for more than 100 ms
		timeout.expires_from_now( boost::posix_time::milliseconds(1000) );
		timeout.async_wait( boost::bind( &SIMPLE_GPS::wait_callback, this, boost::ref(port),
				boost::asio::placeholders::error ) );

		// reset then run the io service to start the asynchronous operation
		io.reset();
		io.run();

//		cout << "read " << asyncBytesRead << endl;

		if (data_available) {
			string_buffer += string(readBuffer.begin(), readBuffer.begin()+=asyncBytesRead);
//			cout << string_buffer << endl;
			int start_index, stop_index;
			while ( (stop_index=string_buffer.find("\n",1))!=string::npos &&
					 (start_index=string_buffer.find("$",0))!=string::npos ) {
				parseLine( string_buffer.substr(start_index, stop_index) );
//				cout << "index: " << index << endl;
//				cout << string_buffer.substr(0, index) << endl;
//				m_Comms.Notify("GPS_SENTENCE", string_buffer.substr(0, index) );
				string_buffer = string_buffer.substr( stop_index, string_buffer.size()-stop_index );
			}
			// print out read data in hexidecimal format
			/*for (int i=0; i<asyncBytesRead; i++) {
				cout << readBuffer[i];
			}
			cout.flush();*/
		}
	}
}
