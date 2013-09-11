/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: WTStatusView.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <sstream>
#include "MBUtils.h"
#include "WTStatusView.h"

using namespace Wt;

boost::mutex data_mutex;
double voltage;

//---------------------------------------------------------
// Constructor

StatusViewApplication::StatusViewApplication(const WEnvironment& env) :
	WApplication(env) {
	setTitle("Status View");

	root()->addWidget(new WText("Voltage: "));
	voltage_ = new WText(root());

	WTimer *timer = new WTimer();
	timer->setInterval(1000);
	timer->timeout().connect(this, &StatusViewApplication::update);
	timer->start();
}

void StatusViewApplication::update() {
	boost::mutex::scoped_lock lock(data_mutex);
	std::stringstream ss;
	ss << voltage;
	voltage_->setText(ss.str());
}

WTStatusView::WTStatusView() {
	wtThread = boost::thread(boost::bind(&WTStatusView::startWT, this));
}

//---------------------------------------------------------
// Destructor

WTStatusView::~WTStatusView() {
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool WTStatusView::OnNewMail(MOOSMSG_LIST &NewMail) {
	boost::mutex::scoped_lock lock(data_mutex);
	MOOSMSG_LIST::iterator p;

	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		std::string key = msg.GetKey();
		if (key == "VOLTAGE") {
			voltage = msg.GetDouble();
		}

	}

	return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool WTStatusView::OnConnectToServer() {
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool WTStatusView::Iterate() {
	return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool WTStatusView::OnStartUp() {
	return (true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void WTStatusView::RegisterVariables() {
	// m_Comms.Register("FOOBAR", 0);
	m_Comms.Register("VOLTAGE", 0);
}

WApplication *createApplication(const WEnvironment& env) {
	/*
	 * You could read information from the environment to decide whether
	 * the user has permission to start a new application
	 */
	return new StatusViewApplication(env);
}

void WTStatusView::startWT() {
	char *myArgv[] = {"hello",
			"--docroot=.",
			"--http-address=127.0.0.1",
			"--http-port=8080"
	};

	WRun(4, myArgv, &createApplication);
}
