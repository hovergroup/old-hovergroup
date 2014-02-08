/************************************************************/
/*    NAME:                                                 */
/*    ORGN: MIT                                             */
/*    FILE: NSFModem.cpp                                    */
/*    DATE: 19-08-2013                                      */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "NSFModem.h"

using namespace std;

//---------------------------------------------------------
// Constructor

NSFModem::NSFModem() :
		m_power_increase_pin_value("/gpio/boardio5/value"),
		m_power_increase_pin_direction("/gpio/boardio5/direction"),
		m_power_decrease_pin_value("/gpio/boardio6/value"),
		m_power_decrease_pin_direction("/gpio/boardio6/direction"),
		m_voltage_sense_pin_value("/gpio/boardio7/value"),
		m_voltage_sense_pin_direction("/gpio/boardio7/direction")
{
	m_state = Starting;

	if (!m_power_increase_pin_direction.is_open()) {
		std::cerr << "Unable to open TX power increase pin (direction)\n";
		exit (1);
	}
	if (!m_power_increase_pin_value.is_open()) {
		std::cerr << "Unable to open TX power increase pin (value)\n";
		exit (1);
	}
	if (!m_power_decrease_pin_direction.is_open()) {
		std::cerr << "Unable to open TX power decrease pin (direction)\n";
		exit (1);
	}
	if (!m_power_decrease_pin_value.is_open()) {
		std::cerr << "Unable to open TX power decrease pin (value)\n";
		exit (1);
	}


	// seems like pins may need to have their state changed to ensure
	// they work correctly

	// configure pins opposite at first
	m_power_increase_pin_direction << "in";
	m_power_increase_pin_direction.flush();
	m_power_decrease_pin_direction << "in";
	m_power_decrease_pin_direction.flush();
	m_voltage_sense_pin_direction << "out";
	m_voltage_sense_pin_direction.flush();

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	// now configure pins to what we want
	m_power_increase_pin_direction << "out";
	m_power_increase_pin_direction.flush();
	m_power_decrease_pin_direction << "out";
	m_power_decrease_pin_direction.flush();
	m_voltage_sense_pin_direction << "in";
	m_voltage_sense_pin_direction.flush();

	// set tx power to maximum
	for (unsigned short int i = 0; i < 32; i++) {
		m_power_increase_pin_value << 0; // set GPIO6 to LOW
		m_power_increase_pin_value.flush();
		boost::this_thread::sleep(boost::posix_time::milliseconds(debounce_time)); // sleep
		m_power_increase_pin_value << 1; // set GPIO6 to HIGH
		m_power_increase_pin_value.flush();
		boost::this_thread::sleep(boost::posix_time::milliseconds(gap_time)); // sleep
	}

	m_requested_power_level = 0;
	m_current_power_level = 0;

	print_power_status();
}

//---------------------------------------------------------
// Destructor

NSFModem::~NSFModem() {
	// release pins
	m_power_increase_pin_value.close();
	m_power_decrease_pin_direction.close();
	m_power_decrease_pin_value.close();
	m_power_decrease_pin_direction.close();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool NSFModem::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;

		std::string key = msg.GetKey();

		if (key == "NSFMODEM_POWER_LEVEL") {
			m_requested_power_level = static_cast<int>(msg.GetDouble());
			if (m_requested_power_level > 31) {
				m_requested_power_level = 31;
				std::cout << "Limiting TX power level to 31.\n";
			} else if (m_requested_power_level < 0) {
				m_requested_power_level = 0;
				std::cout << "Limiting TX power level to 0.\n";
			}
			std::cout << "TX power: " << m_requested_power_level << "/"
					<< m_current_power_level << " (requested/current)\n";
		}
	}

	boost::this_thread::sleep(boost::posix_time::milliseconds(30)); // sleep
	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool NSFModem::OnConnectToServer() {
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	m_state = Running;

	RegisterVariables();

	m_power_write_thread = boost::thread(
			boost::bind(&NSFModem::power_write_loop, this));

	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool NSFModem::Iterate() {
	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool NSFModem::OnStartUp() {
	return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void NSFModem::RegisterVariables() {
	m_Comms.Register("NSFMODEM_POWER_LEVEL", 0);
}

void NSFModem::power_write_loop() {
	while (m_state == Running) {
		// check current power level wrt latest request
		int delta = m_requested_power_level - m_current_power_level;

		if (delta > 0) {
			// power needs to be increased (delta > 0)
			std::cout << "Increasing TX power.\n";
			m_power_increase_pin_value << 0;  // set GPIO5 to LOW
			m_power_increase_pin_value.flush();
			boost::this_thread::sleep(boost::posix_time::milliseconds(debounce_time)); // sleep
			m_power_increase_pin_value << 1; // set GPIO5 to HIGH
			m_power_increase_pin_value.flush();
			m_current_power_level++;
			print_power_status();
		} else if (delta < 0) {
			// power needs to be decreased (delta < 0)
			std::cout << "Decreasing TX power.\n";
			m_power_decrease_pin_value << 0; // set GPIO6 to LOW
			m_power_decrease_pin_value.flush();
			boost::this_thread::sleep(boost::posix_time::milliseconds(debounce_time)); // sleep
			m_power_decrease_pin_value << 1; // set GPIO6 to HIGH
			m_power_decrease_pin_value.flush();
			m_current_power_level--;
			print_power_status();
		} else {
			boost::this_thread::sleep(boost::posix_time::milliseconds(200));
			/*
			 m_power_increase_pin_direction.flush();
			 m_power_increase_pin_value.flush();
			 m_power_decrease_pin_direction.flush();
			 m_power_decrease_pin_value.flush();
			 */
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(gap_time)); // sleep
	}
}

void NSFModem::print_power_status() {
	std::cout << "TX power: current=" << m_current_power_level << ", requested="
			<< m_requested_power_level << "\n";
}
