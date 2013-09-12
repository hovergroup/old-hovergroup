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
#include <map>

using namespace Wt;

boost::mutex data_mutex;
std::map<std::string, ProtoNodeReport> data;

//---------------------------------------------------------
// Constructor

StatusViewApplication::StatusViewApplication(const WEnvironment& env) :
	WApplication(env) {
	setTitle("Status View");
	useStyleSheet("CSSexample.css");

	reDraw(0);
	current_num_vehicles = 1;

	WTimer *timer = new WTimer();
	timer->setInterval(1000);
	timer->timeout().connect(this, &StatusViewApplication::update);
	timer->start();
}

void StatusViewApplication::reDraw(int num_vehicles) {
	root()->clear();

	container_ = new WContainerWidget();
	container_->setStyleClass("CSS-example");
	container_->setWidth(num_vehicles*100 + 50);
	WTable *table = new WTable(container_);
	table->setHeaderCount(1);
	table->setStyleClass("table table-bordered");
	table->rowAt(1)->setStyleClass("info");
    for (int i=1; i<table->rowCount(); i++)
        table->elementAt(i,0)->setStyleClass("code");
//	WGridLayout *grid_ = new WGridLayout();
//	container_->setLayout(grid_);

	for (int column=0; column<num_vehicles+1; column++) {
		for (int row=0; row<5; row++) {
			WString cell = WString("Item ({1}, {2})").arg(row).arg(column);
			WText *t = new WText(cell);
			table->elementAt(row, column)->addWidget(t);
//			grid_->addWidget(t, row, column);
		}
	}

	root()->addWidget(container_);
}

void StatusViewApplication::update() {
	boost::mutex::scoped_lock lock(data_mutex);
//	if (data.size()!=current_num_vehicles) {
//		current_num_vehicles = data.size();
//		reDraw(current_num_vehicles);
//	}

	current_num_vehicles++;
	reDraw(current_num_vehicles);

	//voltage_->setText(ss.str());
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
		if (key == "PROTO_REPORT") {
			ProtoNodeReport pnr;
			if (pnr.ParseFromString(msg.GetString())) {
				data[pnr.name()] = pnr;
			}
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
	m_Comms.Register("PROTO_REPORT", 0);
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
