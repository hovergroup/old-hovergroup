#include <iterator>
#include "kayak_driver.h"
#include "MBUtils.h"

using namespace std;
using namespace boost::asio;
using namespace boost::posix_time;

kayak_driver::kayak_driver() : port(io), timeout(io) {
	// initialize buffers with some size larger than should ever be needed
	writeBuffer = vector<char> (1000, 0);
	readBuffer = vector<char> (1000, 0);

	bytesToWrite = 0;
	buffer_index = 0;
	data_available = false;
	stop_requested = false;

	my_baud_rate = 115200;
	my_port_name = "/dev/ttyO0";

	m_desired_rudder = 0;
	m_desired_thrust = 0;
	newCommand = false;
	m_last_command_time = 0;

	INVERT_RUDDER = false;
	RUDDER_OFFSET = 0;

	m_usingFreewave = false;
	m_radioSetTime = -1;
	m_radioWaitTime = 120;
}

int kayak_driver::roundFloat( double val ) {
	if ( val < 0.0 )
		return (int) floor( val - 0.5 );
	else
		return (int) floor( val + 0.5 );
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool kayak_driver::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	for(p=NewMail.begin(); p!=NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		string key  = msg.GetKey();

		// check for new commands from moosdb
		if (key == "DESIRED_RUDDER") {
			m_desired_rudder = mapRudder( roundFloat( msg.GetDouble() ) );
			newCommand = true;
		} else if (key == "DESIRED_THRUST" ) {
			m_desired_thrust = mapThrust( roundFloat( msg.GetDouble() ) );
			newCommand = true;
		} else if (key == "RADIO_POWER") {
			bool freewave = false;
			if (MOOSToUpper(msg.GetString()) == "FREEWAVE")
				freewave = true;

			// not waiting for confirmation message
			if (m_radioSetTime==-1 ||
					(m_radioSetTime!=-1 && freewave!=m_usingFreewave)) {
				if (freewave) {
					setRadioPower(true);
					m_usingFreewave = true;
				} else {
					setRadioPower(false);
					m_usingFreewave = false;
				}
				m_radioSetTime = MOOSTime();
			} else {
				m_radioSetTime = -1;
			}
		}
	}

	return(true);
}

int kayak_driver::mapRudder( int rudder_command ) {
	rudder_command += RUDDER_OFFSET;
	// invert and offset rudder
	if ( INVERT_RUDDER )
		rudder_command*=-1;

	// limit
	if ( rudder_command > 90 )
		rudder_command = 90;
	else if ( rudder_command < -90 )
		rudder_command = -90;

	return rudder_command;
}

int kayak_driver::mapThrust( int thrust_command ) {
	// limit thrust command
	if ( thrust_command >= 100 )
		return 100;
	else if ( thrust_command <= -100 )
		return -100;\
	else
		return thrust_command;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool kayak_driver::OnConnectToServer()
{
	m_MissionReader.GetConfigurationParam( "BAUD_RATE", my_baud_rate );
	m_MissionReader.GetConfigurationParam( "PORT_NAME", my_port_name );
	m_MissionReader.GetConfigurationParam( "INVERT_RUDDER", INVERT_RUDDER );
	m_MissionReader.GetConfigurationParam( "RUDDER_OFFSET", RUDDER_OFFSET );
	m_MissionReader.GetConfigurationParam("RADIO_WAIT_TIME", m_radioWaitTime);

	RegisterVariables();

	open_port( my_port_name, my_baud_rate );
	//serial_thread = boost::thread(boost::bind(&kayak_driver::serialLoop, this));

	return(true);
}

//------------------------------------------------------------
// Procedure: RegisterVariables

void kayak_driver::RegisterVariables()
{
	m_Comms.Register("DESIRED_RUDDER", 0);
	m_Comms.Register("DESIRED_THRUST", 0);
	m_Comms.Register("RADIO_POWER", 1);
}


void kayak_driver::sendMotorCommands() {
	// format motor commands and write to arduino
//	char tmp [100];
//	int size = sprintf(&tmp[0], "!M=%d,%d\r",
//			m_desired_thrust*10, m_desired_rudder);
	stringstream ss;
	ss << "!M=" << m_desired_thrust*10 << "," << m_desired_rudder << "#";
	writeData( ss.str().c_str(), ss.str().size() );
//	writeData( &tmp[0], size );

//	cout << "sending command string: " << ss.str() << endl;
}

void kayak_driver::setRadioPower(bool freewave) {
	stringstream ss;
	ss << "!R=" << freewave << "#";
	writeData(ss.str().c_str(), ss.str().size());
}

//---------------------------------------------------------
// Procedure: Iterate()

bool kayak_driver::Iterate()
{
	// send at least 4 commands a second
	if ( MOOSTime() > m_last_command_time + .25 || newCommand) {
		sendMotorCommands();
		m_last_command_time = MOOSTime();
		newCommand = false;
	}

	if (m_radioSetTime!=-1 && MOOSTime()>m_radioSetTime+m_radioWaitTime) {
		if (m_usingFreewave)
			setRadioPower(false);
		else
			setRadioPower(true);
		m_usingFreewave = !m_usingFreewave;
		m_radioSetTime = -1;
	}
	return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//      Note: happens before connection is open

bool kayak_driver::OnStartUp()
{
	// I prefer to do nothing here
	return(true);
}

void kayak_driver::open_port( string port_name, int baudRate ) {
	// open the serial port
	port.open(port_name);

	// serial port must be configured after being opened
	port.set_option(serial_port_base::baud_rate(baudRate));
	port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
	port.set_option(serial_port_base::parity(serial_port_base::parity::none));
	port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
	port.set_option(serial_port_base::character_size(8));

	// start the background thread
	serial_thread = boost::thread(boost::bind(&kayak_driver::serialLoop, this));
}

void kayak_driver::close_port() {
	stop_requested = true;
	serial_thread.join();
	port.close();
}

void kayak_driver::writeData( unsigned char *ptr, int length ) {
	writeBufferMutex.lock();
	memcpy(&writeBuffer[bytesToWrite], ptr, length);
	bytesToWrite += length;
	writeBufferMutex.unlock();
}

void kayak_driver::writeData( char * ptr, int length ) {
	writeBufferMutex.lock();
	memcpy(&writeBuffer[bytesToWrite], ptr, length);
	bytesToWrite += length;
	writeBufferMutex.unlock();
}

void kayak_driver::writeData( const char * ptr, int length ) {
	writeBufferMutex.lock();
	memcpy(&writeBuffer[bytesToWrite], ptr, length);
	bytesToWrite += length;
	writeBufferMutex.unlock();
}

void kayak_driver::read_handler(bool& data_available, deadline_timer& timeout,
	const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (error || !bytes_transferred) {
		// no data read
		data_available = false;
		return;
	}
//	cout << "data available: " << bytes_transferred << endl;
	data_available = true;
	buffer_index+=bytes_transferred;
	timeout.cancel();
}

void kayak_driver::wait_callback(serial_port& ser_port, const boost::system::error_code& error)
{
	if (error) {
		// data read, timeout cancelled
		return;
	}
	port.cancel(); // read_callback fires with error
}

void kayak_driver::processWriteBuffer() {
	// take out lock
	writeBufferMutex.lock();
	if ( bytesToWrite > 0 ) {
		// if there is data waiting, copy it to a local buffer
		vector<char> localWriteBuffer( bytesToWrite, 0 );
		memcpy(&localWriteBuffer[0], &writeBuffer[0], bytesToWrite);
		bytesToWrite = 0;
		// release lock to prevent outside write requests from blocking on serial write
		writeBufferMutex.unlock();

//		cout << dec << "writing " << localWriteBuffer.size() << " bytes" << endl;

		// simple synchronous serial write
		port.write_some( buffer(localWriteBuffer, localWriteBuffer.size()) );
	} else {
		// no data to write, release lock
		writeBufferMutex.unlock();
	}
}

int kayak_driver::findLine(int index) {
	for ( int i=index; i<buffer_index; i++ ) {
		if ( readBuffer[i]=='\r' )
			return i;
	}
	return -1;
}

int kayak_driver::processBuffer() {
//	cout << "processing: ";
//	for (int i=0; i<buffer_index; i++ ) {
//		cout << readBuffer[i];
//	}
//	cout << endl;

	int bytesUsed = 0;
	int stopIndex = findLine(bytesUsed);
	if (stopIndex == -1)
		return bytesUsed;
	while (bytesUsed < buffer_index) {
		if (bytesUsed > stopIndex) {
			stopIndex = findLine(bytesUsed);
			if (stopIndex == -1)
				return bytesUsed;
		}
		switch (readBuffer[bytesUsed]) {
		case 'V':
			parseVoltages(bytesUsed, stopIndex);
			bytesUsed = stopIndex;
			break;
		case 'T':
			parseTemperatures(bytesUsed, stopIndex);
			bytesUsed = stopIndex;
			break;
		case 'C':
			parseCurrents(bytesUsed, stopIndex);
			bytesUsed = stopIndex;
			break;
		case 'M':
			parseActuators(bytesUsed, stopIndex);
			bytesUsed = stopIndex;
			break;
		}
		bytesUsed++;
	}
	return bytesUsed;
}

void kayak_driver::shiftBuffer(int shift) {
//	cout << "shifting " << buffer_index-shift << " bytes by " << shift << endl;

	if (shift == 0 || buffer_index == 0)
		return;
	for (int i = shift; i < buffer_index; i++) {
		readBuffer[i - shift] = readBuffer[i];
	}
	buffer_index -= shift;
}

void kayak_driver::parseVoltages(int index, int stopIndex) {
	if (readBuffer[index] == 'V' && readBuffer[index + 1] == '=') {
		int battery_voltage;
		sscanf(&readBuffer[index], "V=%d", &battery_voltage);
		m_Comms.Notify("VOLTAGE", battery_voltage/10.0);
	} else {
		cout << "bad parse" << endl;
	}
}

void kayak_driver::parseTemperatures(int index, int stopIndex) {
	if (readBuffer[index] == 'T' && readBuffer[index + 1] == '=') {
		int heatsink_temp, internal_temp, cpu_temp;
		sscanf(&readBuffer[index], "T=%d,%d,%d", &cpu_temp, &heatsink_temp, &internal_temp);
		m_Comms.Notify("CPU_BOX_TEMP", cpu_temp/10.0);
		m_Comms.Notify("ROBOTEQ_HEATSINK_TEMP", heatsink_temp);
		m_Comms.Notify("ROBOTEQ_INTERNAL_TEMP", internal_temp);
	} else {
		cout << "bad parse" << endl;
	}
}

void kayak_driver::parseCurrents(int index, int stopIndex) {
	if (readBuffer[index] == 'C' && readBuffer[index + 1] == '=') {
		int battery_amps, motor_amps, cpu_amps;
		sscanf(&readBuffer[index], "C=%d,%d,%d", &battery_amps, &motor_amps, &cpu_amps);
		m_Comms.Notify("ROBOTEQ_BATTERY_CURRENT", battery_amps/10.0);
		m_Comms.Notify("ROBOTEQ_MOTOR_CURRENT", motor_amps/10.0);
		m_Comms.Notify("CPU_BOX_CURRENT", cpu_amps/1000.0);
	} else {
		cout << "bad parse" << endl;
	}
}

void kayak_driver::parseActuators(int index, int stopIndex) {
	if (readBuffer[index] == 'M' && readBuffer[index + 1] == '=') {
		int thrust, rudder;
		sscanf(&readBuffer[index], "M=%d,%d", &thrust, &rudder);
		m_Comms.Notify("ARDUINO_THRUST", thrust);
		m_Comms.Notify("ARDUINO_RUDDER", rudder);
	} else {
		cout << "bad parse" << endl;
	}
}

void kayak_driver::serialLoop() {
	while (!stop_requested) {

//		cout << "processing write buffer" << endl;
		processWriteBuffer();

//		cout << "async fun: " << buffer_index << endl;
		// set up an asynchronous read that will read up to 100 bytes, but will return as soon as any bytes area read
		// bytes read will be placed into readBuffer starting at index 0
		port.async_read_some( buffer( &readBuffer[buffer_index], 1000-buffer_index ),
				boost::bind( &kayak_driver::read_handler, this, boost::ref(data_available), boost::ref(timeout),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred ) );
		// setup a timer that will prevent the asynchronous operation for more than 100 ms
		timeout.expires_from_now( boost::posix_time::milliseconds(1000) );
		timeout.async_wait( boost::bind( &kayak_driver::wait_callback, this, boost::ref(port),
				boost::asio::placeholders::error ) );

		// reset then run the io service to start the asynchronous operation
		io.reset();
		io.run();

//		cout << "async fun done" << endl;
		if (data_available) {
			shiftBuffer( processBuffer() );
		}
	}
}
