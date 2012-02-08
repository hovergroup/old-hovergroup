#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/thread/thread_time.hpp>
#include <boost/detail/atomic_count.hpp>
#include <vector>
#include <stdio.h>
#include <numeric>
#include <exception>
#include <stdint.h>
#include <deque>

namespace xbee_api
{
	uint64_t reverseAddress( uint64_t address );

	class NoReceivedDataException : public std::exception {
		virtual const char * what() const throw() {
			return "No received data available";
		}
	};
	
	class SerialPortException : public std::exception {
		virtual const char * what() const throw() {
			return ("Could not open serial port: " + port_name).c_str();
		}
	public:
		SerialPortException( std::string name ) : port_name(name) { }
		~SerialPortException() throw() {}
	private:
		std::string port_name;
	};

	struct RECEIVED_DATA_PACKET {
		RECEIVED_DATA_PACKET ( unsigned char *ptr ) {
			memcpy( &frame_type, ptr, 13 );
		}
	
		unsigned char frame_type;
		unsigned char frame_id;
		uint64_t source_address;
		unsigned short reserved;
		unsigned char receive_options;
		std::vector<unsigned char> data;
	};
		
	struct TRANSMIT_STATUS {
		int transmit_retry_count;
		unsigned char delivery_status;
		// 0x00 = success
		// 0x01 = MAc ACK Failure
		// 0x15 = invalid destination endpoint
		// 0x21 = network ACK failure
		// 0x25 = route not found
		
		unsigned char discovery_status;
		// 0x00 = no discovery overhead
		// 0x02 = route discovery
	};
	
	struct TRANSMIT_REQUEST {
		unsigned char placeHolder;
		TRANSMIT_REQUEST () {
			address = 0x000000000000ffff;
			broadcast_radius = 0;
			disable_acknowledge = false;
			disable_route_discovery = false;
		}
		
		uint64_t address;
		int broadcast_radius;
		// set to 0 for max hops
		
		bool disable_acknowledge;
		bool disable_route_discovery;
		
		std::vector<unsigned char> data;
	};
	
	struct TRANSMIT_REQUEST_PACKET {
		TRANSMIT_REQUEST_PACKET() {
			frame_type = 0x10;
			reserved = 0xfffe;
			broadcast_radius = 0x00;
			transmit_options = 0x00;
		}
		
		TRANSMIT_REQUEST_PACKET( struct TRANSMIT_REQUEST * tr ) {
			frame_type = 0x10;
			reserved = 0xfffe;
			address = reverseAddress(tr->address);
			broadcast_radius = (unsigned char) tr->broadcast_radius;
			transmit_options = 0x01*tr->disable_acknowledge + 0x02*tr->disable_route_discovery;
			//data = tr->data;
		}
		unsigned char frame_type;
		unsigned char frame_id;
		uint64_t address;
		unsigned short reserved;
		unsigned char broadcast_radius;
		unsigned char transmit_options;
		//std::vector<unsigned char> data;
	} __attribute__((packed));
	
	struct RECEIVED_DATA {
		RECEIVED_DATA( RECEIVED_DATA_PACKET * rdp ) {
			source_address = rdp->source_address;
			data = rdp->data;
			switch ( rdp->receive_options ) {
				case 0x01:
					packet_acknowledged = true;
					broadcast_packet = false;
					break;
				case 0x02:
					packet_acknowledged = false;
					broadcast_packet = true;
					break;
				default:
					packet_acknowledged = false;
					broadcast_packet = false;
			}
		}
	
		uint64_t source_address;
		bool packet_acknowledged;
		bool broadcast_packet;
		std::vector<unsigned char> data;
	};
	
	struct NODE_IDENTIFICATION_PACKET {
		unsigned char frame_type;
		uint64_t source_address;
		unsigned short source_network_address;
		unsigned char receive_options;
		unsigned short remote_network_address;
		uint64_t remote_address;
		std::vector<char> NI_string;
		unsigned short remote_parent_address;
	};
	
	struct NODE_IDENTIFICATION {
		unsigned char frame_type;
		uint64_t source_address;
		unsigned short source_network_address;
		unsigned short remote_network_address;
		uint64_t remote_address;
		std::string NI_string;
		unsigned short remote_parent_address;
	};
	
	class XBeeAPI
	{
	public:
		XBeeAPI();
		
		//TRANSMIT_STATUS requestTransmit( struct TRANSMIT_REQUEST tr );
		
		// at queries return -1 on timeout, data otherwise
		// at sets return -1 on timeout, use xbee at response codes otherwise
		int getLocalBaudRate();
		int setLocalBaudRate( int baudRate );
		int getLocalDutyCycle();
		int softwareReset();
		
		void sendNetworkDiscover();
		NODE_IDENTIFICATION sendNodeDiscover( std::string NI_string );
		
		TRANSMIT_STATUS transmitRequest( struct TRANSMIT_REQUEST * tr );
		
		// exceptions need to be implemented here for when emtpy
		int checkNumReceived();
		std::vector<RECEIVED_DATA> fetchAllReceived();
		RECEIVED_DATA fetchLatestReceived();
		
		void openPort();
		void closePort();
		bool setBaudRate( int baudRate );
		bool setPortName( std::string port_name );
		
	private:
		
		// for incoming data
		std::deque<RECEIVED_DATA> received_data_deque;
		boost::mutex received_data_deque_mutex;
	
		// constants --------
		static const int serialReadTimeout = 50;
		static const int shortestCompletePacket = 5;
		
		static const int localATCommandTimeout = 200;
		
		static const unsigned char ATResponseFrameType = 0x88;
		static const unsigned char ModemStatusFrameType = 0x8a;
		static const unsigned char TransmitStatusFrameType = 0x8b;
		static const unsigned char ReceivePacketFrameType = 0x90;
		
		std::vector<unsigned char> BaudRate_ATCommand;
		std::vector<unsigned char> DutyCycle_ATCommand;
		std::vector<unsigned char> SoftwareReset_ATCommand;
		std::vector<unsigned char> NetworkDiscover_ATCommand;
		std::vector<unsigned char> NodeDiscover_ATCommand;
		// ------------------
	
		// serial port functionality
		boost::asio::io_service ioService;
		boost::asio::serial_port serialPort;
		boost::asio::deadline_timer timeout;
		boost::thread readThread;
		std::vector<unsigned char> readBuffer;
		std::string serialPortName;
		int serialPortBaudRate;
		
		// send stack
		std::vector<unsigned char> framePacket( unsigned char *ptr, int length );
		std::vector<unsigned char> basicATPacket( unsigned char frameID, unsigned char *initials);
		bool waitForATResponse( unsigned char frameID, int timeout );
		unsigned char getNewFrameID();
		// --
		boost::mutex at_response_mutex;
		boost::condition_variable at_response_cond;
		unsigned char at_response_frameID;
		boost::detail::atomic_count ref_frameID;
		// local AT command query data storage
		int local_integer_store;
		// local AT command set response
		int local_command_status;
		// --
		TRANSMIT_STATUS local_transmit_status;
		
		// adding to write buffer
		std::vector<unsigned char> writeBuffer;
		boost::mutex writeBufferMutex;
		void appendToWriteBuffer( unsigned char *ptr, int length );
		int bytesToWrite;
		
		// serial port thread
		void serialLoop();
		void processWriteBuffer();
		bool data_available;
		int asyncBytesRead;
		void read_handler(bool& data_available, boost::asio::deadline_timer& timeout, 
			const boost::system::error_code& error, std::size_t bytes_transferred);
		void wait_callback(boost::asio::serial_port& ser_port, const boost::system::error_code& error);
		
		// receive stack
		unsigned int frameFinder(int bytesRead);
		void parsePacket( int frame_type_index, int data_length );
		void parseATPacket( int frame_type_index, int data_length ); 
		void signalFrameID( unsigned char frame_id );
		// --
		void parseBaudRateATCommand( int frame_type_index, int data_length );
		void parseDutyCycleATCommand( int frame_type_index, int data_length );
		void parseModemStatus( int frame_type_index, int data_length );
		void parseMinimalATAck( int frame_type_index, int data_length );
		void parseTransmitStatus( int frame_type_index, int data_length );
		void parseReceivePacket( int frame_type_index, int data_length );
		void parseNetworkDiscover( int frame_type_index, int data_length );
	};
}
