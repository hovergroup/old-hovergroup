#include "XBeeAPI.h"

using namespace std;
using namespace boost::asio;
using namespace boost::posix_time;

namespace xbee_api
{
	XBeeAPI::XBeeAPI() : serialPort(ioService), timeout(ioService), ref_frameID(0) {
		writeBuffer = vector<unsigned char> (1000, 0);
		readBuffer = vector<unsigned char> (1000, 0);
		
		BaudRate_ATCommand.push_back('B');
		BaudRate_ATCommand.push_back('D');
		
		DutyCycle_ATCommand.push_back('D');
		DutyCycle_ATCommand.push_back('C');
		
		SoftwareReset_ATCommand.push_back('F');
		SoftwareReset_ATCommand.push_back('R');
		
		NetworkDiscover_ATCommand.push_back('N');
		NetworkDiscover_ATCommand.push_back('D');
		
		NodeDiscover_ATCommand.push_back('D');
		NodeDiscover_ATCommand.push_back('N');
		
		bytesToWrite = 0;
		asyncBytesRead = 0;
		data_available = false;
		
		serialPortName = "/dev/ttyUSB0";
		serialPortBaudRate = 9600;
	}
	
	bool XBeeAPI::setBaudRate( int baudRate ) {
		if ( serialPort.is_open() ) {
			serialPort.set_option(serial_port_base::baud_rate(baudRate));
			serialPortBaudRate = baudRate;
			return true;
		} else {
			serialPortBaudRate = baudRate;
			return true;
		}
	}
	
	bool XBeeAPI::setPortName( string port_name ) {
		if ( !serialPort.is_open() ) {
			serialPortName = port_name;
			return true;
		} else
			return false;
	}
	
	void XBeeAPI::closePort() {
		if ( serialPort.is_open() ) {
			readThread.join();
			serialPort.close();
		} else {
			cout << "port already closed" << endl;
		}
	}
	
	void XBeeAPI::openPort() {
		if ( !serialPort.is_open() ) {
			try {
				serialPort.open(serialPortName);
			} catch ( exception& e ) {
				throw SerialPortException( serialPortName );
			}

			cout << "opening " << serialPortName << " at baud rate " << serialPortBaudRate << endl;

			serialPort.set_option(serial_port_base::baud_rate(serialPortBaudRate));
			serialPort.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
			serialPort.set_option(serial_port_base::parity(serial_port_base::parity::none));
			serialPort.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
			serialPort.set_option(serial_port_base::character_size(8));
			
			readThread = boost::thread(boost::bind(&XBeeAPI::serialLoop, this));
		} else {
			cout << "port already open" << endl;
		}
	}
	
	unsigned char XBeeAPI::getNewFrameID() {
		++ref_frameID;
		return (unsigned char) ref_frameID;
	}
	
	std::vector<unsigned char> XBeeAPI::basicATPacket( unsigned char frameID, unsigned char *initials ) {
		vector<unsigned char> packet (4, 0);
		packet[0] = 0x08;
		packet[1] = frameID;
		memcpy(&packet[2], initials, 2);
		return packet;
	}
	
	TRANSMIT_STATUS XBeeAPI::transmitRequest( struct TRANSMIT_REQUEST * tr ) {
	
		TRANSMIT_REQUEST_PACKET trp( tr );
		
		trp.frame_id = getNewFrameID();
		
		
		vector<unsigned char> packet (14+tr->data.size(), 0);
		memcpy(&packet[0], &trp.frame_type, 14);
		memcpy(&packet[14], &tr->data[0], tr->data.size() );
		
		cout << "packet: " << endl;
		for (int i=0; i<packet.size(); i++) {
			cout << hex << (int) packet[i] << " ";
		}
		cout << endl;
		
		vector<unsigned char> complete_packet = framePacket( &packet[0], packet.size() );
		
		cout << "packet (framed): " << endl;
		for (int i=0; i<complete_packet.size(); i++) {
			cout << hex << (int) complete_packet[i] << " ";
		}
		cout << dec << endl;
		
		appendToWriteBuffer( &complete_packet[0], complete_packet.size() );
		
		TRANSMIT_STATUS ts;
		
		return ts;
	}
	
	uint64_t reverseAddress( uint64_t address ) {
		uint64_t result;
		result = 
			( (address >> 56) & 0x00000000000000ff ) + 
			( (address >> 40) & 0x000000000000ff00 ) + 
			( (address >> 24) & 0x0000000000ff0000 ) + 
			( (address >> 8 ) & 0x00000000ff000000 ) + 
			( (address << 8 ) & 0x000000ff00000000 ) + 
			( (address << 24) & 0x0000ff0000000000 ) + 
			( (address << 40) & 0x00ff000000000000 ) +
			( (address << 56) & 0xff00000000000000 );
		return result;
	}
	
	int XBeeAPI::checkNumReceived() {
		received_data_deque_mutex.lock();
		int result = received_data_deque.size();
		received_data_deque_mutex.unlock();
		return result;
	}
	
	RECEIVED_DATA XBeeAPI::fetchLatestReceived() {
		received_data_deque_mutex.lock();
		if ( received_data_deque.size() == 0 ) {
			received_data_deque_mutex.unlock();
			// throw exception
			throw NoReceivedDataException ();
		} else {
			RECEIVED_DATA result = received_data_deque.front();
			received_data_deque.pop_front();
			received_data_deque_mutex.unlock();
			return result;
		}
	}
	
	vector<RECEIVED_DATA> XBeeAPI::fetchAllReceived() {
		received_data_deque_mutex.lock();
		if ( received_data_deque.size() == 0 ) {
			received_data_deque_mutex.unlock();
			// throw exception
			throw NoReceivedDataException();
		} else {
			deque<RECEIVED_DATA>::iterator it = received_data_deque.begin();
			vector<RECEIVED_DATA> result;
			while ( it != received_data_deque.end() )
				result.push_back( *it++ );
			received_data_deque.clear();
			received_data_deque_mutex.unlock();
			return result;
		}
	}
	
	int XBeeAPI::getLocalBaudRate() {
		unsigned char myFrameID = getNewFrameID();
		vector<unsigned char> packet = basicATPacket( myFrameID, &BaudRate_ATCommand[0] );
		vector<unsigned char> complete_packet = framePacket( &packet[0], 4 );
		
		appendToWriteBuffer( &complete_packet[0], complete_packet.size() );
		
		if ( waitForATResponse( myFrameID, localATCommandTimeout ) ) {
			return local_integer_store;
		} else {
			// cout << "wasn't signalled" << endl;
			return -1;
		}
	}
	
	int XBeeAPI::getLocalDutyCycle() {
		unsigned char myFrameID = getNewFrameID();
		vector<unsigned char> packet = basicATPacket( myFrameID, &DutyCycle_ATCommand[0] );
		vector<unsigned char> complete_packet = framePacket( &packet[0], 4);
		
		appendToWriteBuffer( &complete_packet[0], complete_packet.size() );
		
		if ( waitForATResponse( myFrameID, localATCommandTimeout ) ) {
			return local_integer_store;
		} else {
			return -1;
		}
	}
	
	int XBeeAPI::setLocalBaudRate( int baudRate ) {
		unsigned char myFrameID = getNewFrameID();
		vector<unsigned char> packet = basicATPacket( myFrameID, &BaudRate_ATCommand[0] );
		
		if ( baudRate == 1200 )
			packet.push_back(0x00);
		else if ( baudRate == 2400 )
			packet.push_back(0x01);
		else if ( baudRate == 4800 )
			packet.push_back(0x02);
		else if ( baudRate == 9600 )
			packet.push_back(0x03);
		else if ( baudRate == 19200 )
			packet.push_back(0x04);
		else if ( baudRate == 38400 )
			packet.push_back(0x05);
		else if ( baudRate == 57600 )
			packet.push_back(0x06);
		else if ( baudRate == 115200 )
			packet.push_back(0x07);
		else if ( baudRate == 230400 )
			packet.push_back(0x08);
		else {
			cout << "bad baud rate" << endl;
			return -1;
		}
		vector<unsigned char> complete_packet = framePacket( &packet[0], 5 );
		
		appendToWriteBuffer( &complete_packet[0], complete_packet.size() );
		
		if ( waitForATResponse( myFrameID, localATCommandTimeout ) ) {
			return local_command_status;
		} else {
			return -1;
		}
	}
	
	int XBeeAPI::softwareReset() {
		unsigned char myFrameID = getNewFrameID();
		vector<unsigned char> packet = basicATPacket( myFrameID, &SoftwareReset_ATCommand[0] );
		vector<unsigned char> complete_packet = framePacket( &packet[0], packet.size() );
		
		appendToWriteBuffer( &complete_packet[0], complete_packet.size() );
		
		if ( waitForATResponse( myFrameID, localATCommandTimeout ) ) {
			return local_command_status;
		} else {
			return -1;
		}
	}
	
	bool XBeeAPI::waitForATResponse( unsigned char frameID, int timeout ) {
		ptime p ( microsec_clock::universal_time() + millisec(timeout) );
		boost::unique_lock<boost::mutex> lock(at_response_mutex);
		bool was_signaled = true;
		while ( at_response_frameID != frameID && was_signaled ) {
			was_signaled = at_response_cond.timed_wait(lock, p);
		}
		return was_signaled;
	}
	
	void XBeeAPI::parseBaudRateATCommand( int frame_type_index, int data_length ) {
		if ( data_length == 9 ) {
			// response to baud rate query
			switch ( readBuffer[frame_type_index+8] ) {
				case 0x00:
					// cout << "1200" << endl;
					local_integer_store = 1200;
					break;
				case 0x01:
					// cout << "2400" << endl;
					local_integer_store = 2400;
					break;
				case 0x02:
					// cout << "4800" << endl;
					local_integer_store = 4800;
					break;
				case 0x03:
					// cout << "9600" << endl;
					local_integer_store = 9600;
					break;
				case 0x04:
					// cout << "19200" << endl;
					local_integer_store = 19200;
					break;
				case 0x05:
					// cout << "38400" << endl;
					local_integer_store = 38400;
					break;
				case 0x06:
					// cout << "57600" << endl;
					local_integer_store = 57600;
					break;
				case 0x07:
					// cout << "115200" << endl;
					local_integer_store = 115200;
					break;
				case 0x08:
					// cout << "230400" << endl;
					local_integer_store = 230400;
					break;
				default:
					cout << "unrecogized baud rate: " << hex << (int) readBuffer[frame_type_index+5] << endl;
					break;
			}
			signalFrameID( readBuffer[frame_type_index+1]);
		} else if ( data_length == 5 ) {
			// response to baud rate set command
			parseMinimalATAck( frame_type_index, data_length);
		} else {
			cout << "wrong length for baud rate response - got " << data_length << endl;
		}
	}
	
	void XBeeAPI::parseDutyCycleATCommand( int frame_type_index, int data_length ) {
		if ( data_length == 6 ) {
			local_integer_store = (int) readBuffer[frame_type_index+8];
			signalFrameID( readBuffer[frame_type_index+1]);
		} else {
			cout << "wrong length for duty cycle response - got " << data_length << endl;
		}
	}
	
	void XBeeAPI::parseMinimalATAck( int frame_type_index, int data_length ) {
		if ( data_length == 5 ) {
			local_command_status = (int) readBuffer[frame_type_index+4];
			signalFrameID( readBuffer[frame_type_index+1]);
		} else {
			cout << "wrong length for minimal at ack - got " << data_length << endl;
		}
	}
	
	void XBeeAPI::signalFrameID( unsigned char frame_id ) {
		{
			boost::lock_guard<boost::mutex> lock( at_response_mutex );
			at_response_frameID = frame_id;
		}
		at_response_cond.notify_all();
	}
	
	void XBeeAPI::parseATPacket( int frame_type_index, int data_length ) {
		if ( !memcmp( &readBuffer[frame_type_index+2], &BaudRate_ATCommand[0], 2 ) ) {
			cout << "baud rate packet" << endl;
			parseBaudRateATCommand( frame_type_index, data_length );
		} else if ( !memcmp( &readBuffer[frame_type_index+2], &DutyCycle_ATCommand[0], 2 ) ) {
			cout << "duty cycle packet" << endl;
			parseDutyCycleATCommand( frame_type_index, data_length );
		} else if ( !memcmp( &readBuffer[frame_type_index+2], &SoftwareReset_ATCommand[0], 2 ) ) {
			cout << "software reset packet" << endl;
			parseMinimalATAck( frame_type_index, data_length );
		} else if ( !memcmp( &readBuffer[frame_type_index+2], &NetworkDiscover_ATCommand[0], 2 ) ) {
			cout << "network discover at packet" << endl;
			parseNetworkDiscover( frame_type_index, data_length);
		} else {
			cout << "unrecognized at packet: " << readBuffer[frame_type_index+2] << 
				readBuffer[frame_type_index+3] << "  with length: " << dec << data_length << endl;
			for (int i=0; i<data_length; i++) {
				cout << hex << (int) readBuffer[frame_type_index+i] << " ";
			}
			cout << dec << endl;
		}
	}
	
	void XBeeAPI::parseModemStatus( int frame_type_index, int data_length ) {
		if ( data_length != 2 ) {
			cout << "wrong length for modem status: " << data_length << endl;
		} else {
			switch ( readBuffer[frame_type_index+1] ) {
				case 0x00:
					cout << "hardware reset" << endl;
					break;
				case 0x01:
					cout << "watchdog timer reset" << endl;
					break;
				case 0x0b:
					cout << "network woke up" << endl;
					break;
				case 0x0c:
					cout << "network went to sleep" << endl;
					break;
				default:
					cout << "unrecognized modem status" << endl;
					break;
			}
		}
	}
	
	void XBeeAPI::parseTransmitStatus( int frame_type_index, int data_length ) {
		if ( data_length != 7 ) {
			cout << "wrong length for transmit status: " << data_length << endl;
		} else {
			local_transmit_status.transmit_retry_count = (int) readBuffer[frame_type_index+4];
			local_transmit_status.delivery_status = readBuffer[frame_type_index+5];
			local_transmit_status.discovery_status = readBuffer[frame_type_index+6];
			signalFrameID( readBuffer[frame_type_index+1] );
		}
	}
	
	void XBeeAPI::parseReceivePacket( int frame_type_index, int data_length ) {
		if ( data_length < 14 ) {
			cout << "received data packet too short: " << data_length << endl;
		} else {
			RECEIVED_DATA_PACKET rdp ( &readBuffer[frame_type_index] );
			received_data_deque_mutex.lock();
			received_data_deque.push_back( RECEIVED_DATA( &rdp ) );
			received_data_deque_mutex.unlock();
			cout << "new received data placed in storage" << endl;
		}
	}
	
	void XBeeAPI::parsePacket( int frame_type_index, int data_length ) {
		switch ( readBuffer[frame_type_index] ) { // frame type
			case ATResponseFrameType:
				cout << "got at response" << endl;
				parseATPacket( frame_type_index, data_length );
				break;
			
			case ModemStatusFrameType:
				cout << "got modem status" << endl;
				parseModemStatus( frame_type_index, data_length );
				break;
				
			case TransmitStatusFrameType:
				cout << "got transmit status" << endl;
				parseTransmitStatus( frame_type_index, data_length );
				break;
				
			case ReceivePacketFrameType:
				cout << "received data packet" << endl;
				parseReceivePacket( frame_type_index, data_length );
				break;
				
			default:
				cout << "unrecognized frame type: " << hex << (int) readBuffer[frame_type_index] << dec << endl;
				break;
		}
	}
	
	// place a contiguous API packet in an API frame
	vector<unsigned char> XBeeAPI::framePacket( unsigned char *ptr, int length ) {
		vector<unsigned char> result (length+4, 0);
		memcpy(&result[3], ptr, length);
		result[0] = 0x7e;
		result[1] = length >> 8;
		result[2] = length;
		result[length+3] = 0xff - (unsigned char) accumulate( result.begin()+3, result.end()-1, 0 );
		// checksum needs to be checked
		return result;
	}
	
	// append a contiguous block of memory to the write buffer
	void XBeeAPI::appendToWriteBuffer( unsigned char *ptr, int length ) {
		cout << "appending " << length << " bytes to write buffer" << endl;
		writeBufferMutex.lock();
		memcpy(&writeBuffer[bytesToWrite], ptr, length);
		bytesToWrite += length;
		writeBufferMutex.unlock();
	}
	
	void XBeeAPI::serialLoop() {
		unsigned int bytesRead = 0;
		while (1) {
			boost::this_thread::interruption_point();
			
			processWriteBuffer();
			
			if ( bytesRead >= readBuffer.size()-100 )
				bytesRead = 0;
				
				//buffer( &readBuffer[bytesRead], readBuffer.size()-bytesRead );
				
			serialPort.async_read_some( buffer( &readBuffer[bytesRead], readBuffer.size()-bytesRead ), 
				boost::bind( &XBeeAPI::read_handler, this, boost::ref(data_available), boost::ref(timeout), 
					boost::asio::placeholders::error, 
					boost::asio::placeholders::bytes_transferred ) );
			timeout.expires_from_now( boost::posix_time::milliseconds(serialReadTimeout) );
			timeout.async_wait( boost::bind( &XBeeAPI::wait_callback, this, boost::ref(serialPort), 
				boost::asio::placeholders::error ) );
				
			ioService.reset();
			ioService.run();
			
			if (data_available) {
				bytesRead += asyncBytesRead;
				
				unsigned int usedSoFar = 1;
				while (usedSoFar > 0) {
					usedSoFar = frameFinder(bytesRead);
					if ( usedSoFar > 0 ) {
						for (unsigned int i=usedSoFar; i<bytesRead; i++) {
							readBuffer[i-usedSoFar] = readBuffer[i];
						}
						bytesRead -= usedSoFar;
					}
				}
			}
		}
	}
	
	unsigned int XBeeAPI::frameFinder(int bytesRead) {
		if ( bytesRead < shortestCompletePacket ) {
			return 0;
		}
		unsigned int usedSoFar = 0; // is a length
		
		for (int startLocation = 0; startLocation < bytesRead - shortestCompletePacket; startLocation++ ) {
			if ( readBuffer[startLocation] == 0x7e ) {
				// possible packet start, check its claimed length
				int targetLength = (readBuffer[startLocation+1] << 8) + readBuffer[startLocation+2];
				//cout << "target data length = " << targetLength << "  have read " << bytesRead - startLocation - 4 << endl;
				
				if ( targetLength + 4 <= bytesRead - startLocation ) {
					
					// cout << "received: ";
					// for (int i=startLocation; i<targetLength+4; i++) {
						// cout << hex << (int) readBuffer[i] << " ";
					// }
					// cout << dec << endl;
				
					// enough bytes have been read, check crc
					unsigned char crc = (unsigned char) accumulate( readBuffer.begin()+startLocation+3, 
						readBuffer.begin()+startLocation+targetLength+4, 0 );
					//cout << "calculated crc as " << hex << (int) crc << endl;
					if ( crc == 0xff ) {
						// crc checks, found a packet
						usedSoFar = startLocation+targetLength+4; // hope this is right
						
						parsePacket( startLocation+3, targetLength );
					}
				}
			}
		}
		return usedSoFar;
	}
	
	void XBeeAPI::read_handler(bool& data_available, deadline_timer& timeout, 
		const boost::system::error_code& error, std::size_t bytes_transferred) {
		if (error || !bytes_transferred) {
			// no data read
			data_available = false;
			return;
		}
		//cout << "data read: " << bytes_transferred << endl;
		data_available = true;
		asyncBytesRead = bytes_transferred;
		timeout.cancel();
	}

	void XBeeAPI::wait_callback(serial_port& ser_port, const boost::system::error_code& error) {
		if (error) {
			// data read, timeout cancelled
			//cout << "cancelling timeout" << endl;
			return;
		}
		//cout << "cancelling" << endl;
		serialPort.cancel(); // read_callback fires with error
	}
	
	void XBeeAPI::processWriteBuffer() {
		writeBufferMutex.lock();
		if ( bytesToWrite > 0 ) {
			vector<unsigned char> localWriteBuffer( bytesToWrite, 0 );
			memcpy(&localWriteBuffer[0], &writeBuffer[0], bytesToWrite);
			//cout << "found " << bytesToWrite << " bytes to write" << endl;
			bytesToWrite = 0;
			writeBufferMutex.unlock();
			
			cout << "writing " << localWriteBuffer.size() << " bytes" << endl;
			
			serialPort.write_some( buffer(localWriteBuffer, localWriteBuffer.size()) );
		} else {
			writeBufferMutex.unlock();
		}
	}
	
	void XBeeAPI::sendNetworkDiscover() {
		unsigned char myFrameID = getNewFrameID();
		vector<unsigned char> packet = basicATPacket( myFrameID, &NetworkDiscover_ATCommand[0] );
		vector<unsigned char> complete_packet = framePacket( &packet[0], 4 );
		appendToWriteBuffer( &complete_packet[0], complete_packet.size() );
	}
	
	void XBeeAPI::parseNetworkDiscover( int frame_type_index, int data_length ) {
		int nodeStringLength = data_length - 24;
		uint64_t address;
		memcpy(&address, &readBuffer[frame_type_index+7], 8);
		address = reverseAddress( address );
		vector<char> nodeID_cstr (nodeStringLength, 0x00);
		memcpy(&nodeID_cstr[0], &readBuffer[frame_type_index+15], nodeStringLength );
		string nodeID (&nodeID_cstr[0], nodeStringLength);
		cout << "found node with address: " << hex << address << dec << "  nodeID: " << nodeID << endl;
	}
	
	NODE_IDENTIFICATION XBeeAPI::sendNodeDiscover( string NI_string ) {
		unsigned char myFrameID = getNewFrameID();
		vector<unsigned char> packet = basicATPacket( myFrameID, &NodeDiscover_ATCommand[0] );
		for (int i=0; i<NI_string.size(); i++) {
			packet.push_back(NI_string[i]);
		}
		vector<unsigned char> complete_packet = framePacket( &packet[0], packet.size() );
		appendToWriteBuffer( &complete_packet[0], complete_packet.size() );
		NODE_IDENTIFICATION NI;
		return NI;
	}
}
