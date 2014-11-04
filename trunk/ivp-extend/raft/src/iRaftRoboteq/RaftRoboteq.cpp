/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RaftRoboteq.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "RaftRoboteq.h"

using namespace std;

using boost::asio::ip::tcp;
using namespace boost::asio;

//---------------------------------------------------------
// Constructor

RaftRoboteq::RaftRoboteq() :
                sock(io_service),
                deadline_timer(io_service) {
    tcpReadBuffer = vector<char>(1000, 0);
}

//---------------------------------------------------------
// Destructor

RaftRoboteq::~RaftRoboteq() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RaftRoboteq::OnNewMail(MOOSMSG_LIST &NewMail) {
    MOOSMSG_LIST::iterator p;

    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
    }

    return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RaftRoboteq::OnConnectToServer() {

    return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool RaftRoboteq::Iterate() {
//    boost::system::error_code error;
//    boost::asio::streambuf response;
//    boost::asio::read_until(sock, response, "\n");
//    //int n = sock.read_some(boost::asio::buffer(tcpReadBuffer, 100));
//    //if (n > 0) {
//    std::istream response_stream(&response);
//    cout << response_stream.rdbuf();
        //cout << string(tcpReadBuffer.begin(), tcpReadBuffer.begin() += n) << endl;
    //}

//    ssize_t n = recv(m_tcp_sockfd, (void*)&tcpReadBuffer[0], 1000, 0);
//    //int n = read(m_tcp_sockfd, &tcpReadBuffer[0], 1000);
//    //if (n < 0) {
//    //    std::cout << "error reading from socket" << std::endl;
//    //    return false;
//    //}
//    if (n > 0) {
//        string_buffer += std::string(tcpReadBuffer.begin(),
//                tcpReadBuffer.begin() += n);
//
//        cout << string_buffer;
//        string_buffer = "";
//    }
//    cout << endl;

    return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool RaftRoboteq::OnStartUp() {
    string address = "192.168.1.51";
    string port = "5001";
    m_MissionReader.GetConfigurationParam("address", address);
    m_MissionReader.GetConfigurationParam("port", port);

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), address, port);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    sock.connect(*iterator);

    start_read();
    start_write();
    io_thread = boost::thread(boost::bind(&RaftRoboteq::io_loop, this));

    return (true);
}

void RaftRoboteq::io_loop() {
    cout << "Running io service" << endl;
    io_service.run();
    cout << "Finished running io service" << endl;
}

void RaftRoboteq::start_read() {
    boost::asio::async_read_until(sock, input_buffer, '\n',
            boost::bind(&RaftRoboteq::handle_read, this, _1));
}

void RaftRoboteq::handle_read(const boost::system::error_code& ec) {
    if (!ec) {
        // Extract the newline-delimited message from the buffer.
        std::string line;
        std::istream is(&input_buffer);
        cout << is.rdbuf() << endl;

        start_read();
    } else {
        cout << "Error on receive: " << ec.message() << endl;
    }
}

void RaftRoboteq::start_write() {
    boost::asio::async_write(sock, boost::asio::buffer("hello\n", 6),
            boost::bind(&RaftRoboteq::handle_write, this, _1));
}

void RaftRoboteq::handle_write(const boost::system::error_code& ec) {
    if (!ec) {
        deadline_timer.expires_from_now(boost::posix_time::seconds(1));
        deadline_timer.async_wait(boost::bind(&RaftRoboteq::start_write, this));
    } else {
        cout << "Write on write: " << ec.message() << endl;
    }
}
