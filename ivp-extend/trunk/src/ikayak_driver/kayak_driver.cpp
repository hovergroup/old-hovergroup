#include <iterator>
#include "kayak_driver.h"
#include "MBUtils.h"

using namespace std;
using namespace boost::asio;
using namespace boost::posix_time;

kayak_driver::kayak_driver() : port(io), timeout(io) {
	// initialize buffers with some size larger than should ever be needed
	writeBuffer = vector<unsigned char> (1000, 0);
	readBuffer = vector<unsigned char> (1000, 0);

	bytesToWrite = 0;
	asyncBytesRead = 0;
	data_available = false;
	stop_requested = false;

	my_baud_rate = 115200;
	my_port_name = "/dev/ttyO0";

	string_buffer = "";

	m_desired_rudder = 0;
	m_desired_thrust = 0;
	newCommand = false;
	m_last_command_time = 0;

	INVERT_RUDDER = false;
	RUDDER_OFFSET = 0;
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
			m_desired_rudder = mapRudder( (int) msg.GetDouble() );
			newCommand = true;
		} else if (key == "DESIRED_THRUST" ) {
			m_desired_thrust = mapThrust( (int) msg.GetDouble() );
			newCommand = true;
		}
	}

	return(true);
}

int kayak_driver::mapRudder( int rudder_command ) {
	// invert and offset rudder
	if ( INVERT_RUDDER )
		rudder_command*=-1;
	rudder_command += RUDDER_OFFSET;

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
		return -100;
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

	// I prefer to read my config file here, so I can be sure I finish reading it before doing anything else
//	STRING_LIST sParams;
//	m_MissionReader.EnableVerbatimQuoting(false);
//	m_MissionReader.GetConfiguration(GetAppName(), sParams);
//
//	STRING_LIST::iterator p;
//	for (p=sParams.begin(); p!=sParams.end(); p++) {
//		string sLine = *p;
//		string sVarName = MOOSChomp(sLine, "=");
//		sLine = stripBlankEnds(sLine);
//
//		// this is the variable name we found
//		cout << sVarName << endl;
//
//		// match names to what you expect, and parse the values
//		if (MOOSStrCmp(sVarName, "BAUD_RATE")) {
//			if(!strContains(sLine, " "))
//				my_baud_rate = boost::lexical_cast<int>(stripBlankEnds(sLine));
//		} else if (MOOSStrCmp(sVarName, "PORT_NAME")) {
//			if(!strContains(sLine, " "))
//				my_port_name = stripBlankEnds(sLine);
//		}else if ( sVarName == "INVERT_RUDDER" ) {
//			if(!strContains(sLine, " ")) {
//				int tmp = atoi(stripBlankEnds(sLine).c_str());
//				if ( tmp==0 )
//					INVERT_RUDDER = false;
//				else if ( tmp==1 )
//					INVERT_RUDDER = true;
//			}
//		} else if ( sVarName == "RUDDER_OFFSET" ) {
//			if(!strContains(sLine, " "))
//				RUDDER_OFFSET = atoi(stripBlankEnds(sLine).c_str());
//		}
//	}

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
}


void kayak_driver::sendMotorCommands() {
	// format motor commands and write to arduino
	char tmp [100];
	int size = sprintf(&tmp[0], "mtr:thrust=%d,angle=%d\n",
			m_desired_thrust, m_desired_rudder);
	writeData( &tmp[0], size );

//	cout << "sending command string: " << string(tmp) << endl;
}

//---------------------------------------------------------
// Procedure: Iterate()

bool kayak_driver::Iterate()
{
	// send at least 4 commands a second
	if ( MOOSTime() > m_last_command_time + .25 ) {
		sendMotorCommands();
		m_last_command_time = MOOSTime();
		newCommand = false;
	}
//	m_Comms.Notify("VARIABLE_NAME", "I did something.");
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

void kayak_driver::read_handler(bool& data_available, deadline_timer& timeout,
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
		vector<unsigned char> localWriteBuffer( bytesToWrite, 0 );
		memcpy(&localWriteBuffer[0], &writeBuffer[0], bytesToWrite);
		bytesToWrite = 0;
		// release lock to prevent outside write requests from blocking on serial write
		writeBufferMutex.unlock();

//		cout << endl << dec << "writing " << localWriteBuffer.size() << " bytes" << endl;

		// simple synchronous serial write
		port.write_some( buffer(localWriteBuffer, localWriteBuffer.size()) );
	} else {
		// no data to write, release lock
		writeBufferMutex.unlock();
	}
}

// split string into substrings using provided tokens
vector<string> kayak_driver::tokenizeString( string message, string tokens ) {
	char * cstr = new char [message.size() + 1];
	strcpy(cstr, message.c_str());
	char * ctokens = new char [tokens.size()+1];
	strcpy(ctokens, tokens.c_str());

	char * pch;
	vector<string> subs;

	pch = strtok(cstr,ctokens);
	while (pch != NULL) {
		subs.push_back(string(pch));
		pch  = strtok(NULL,ctokens);
	}

	delete cstr;
	delete ctokens;

	return subs;
}

void kayak_driver::parseSensors( string msg ) {
//	cout << msg << endl;
	vector<string> subs = tokenizeString( msg, ",=" );
	double voltage = atoi(subs[1].c_str())/1000.0;
	m_Comms.Notify("VOLTAGE", voltage);
//	cout << heading << " " << voltage << endl;
}

void kayak_driver::parseLine( string msg ) {
	if ( msg.find("voltage") != -1 ) {
		int index = msg.find("voltage");
		parseSensors( msg.substr(index, msg.length()-index) );
//		cout << "found sensors: " << index << endl;
	} else {
		m_Comms.Notify("ARDUINO_MESSAGES", msg);
		cout << "WARNING: Unhandled Arduino message: " << msg << endl;
	}
}

void kayak_driver::serialLoop() {
	while (!stop_requested) {

		processWriteBuffer();

		// set up an asynchronous read that will read up to 100 bytes, but will return as soon as any bytes area read
		// bytes read will be placed into readBuffer starting at index 0
		port.async_read_some( buffer( &readBuffer[0], 1000 ),
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

		if (data_available) {
			string_buffer += string(readBuffer.begin(), readBuffer.begin()+=asyncBytesRead);
//			cout << string_buffer << endl;
			if ( string_buffer.find('\n',1)!=string::npos ) {
//				cout << string_buffer << endl;
				int index = string_buffer.find('\n',1);
				parseLine( string_buffer.substr(0, index) );
				string_buffer = string_buffer.substr( index, string_buffer.size()-index );
			}
		}
	}
}
