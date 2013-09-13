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
using namespace std;

boost::mutex data_mutex;
std::vector<std::string> vnames;
std::map<std::string, double> report_ages;
std::map<std::string, ProtoNodeReport> data;

const int NUM_ROWS = 6;
const int AGE_ROW = 1;
const int HEALTH_ROW = 2;
const int VOLTAGE_ROW = 3;
const int BATT_PERCENT_ROW = 4;
const int GPS_QUALITY_ROW = 5;

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
	tableTexts.clear();

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
		for (int row=0; row<NUM_ROWS; row++) {
			WText *t = new WText();
			tableTexts[pair<int,int>(row,column)] = t;
			table->elementAt(row, column)->addWidget(t);
//			grid_->addWidget(t, row, column);
		}
	}

	tableTexts[pair<int,int>(AGE_ROW,0)]->setText("Age");
	tableTexts[pair<int,int>(HEALTH_ROW,0)]->setText("Health");
	tableTexts[pair<int,int>(VOLTAGE_ROW,0)]->setText("Voltage");
	tableTexts[pair<int,int>(BATT_PERCENT_ROW,0)]->setText("Batt. %");
	tableTexts[pair<int,int>(GPS_QUALITY_ROW,0)]->setText("GPS Quality");

	for (int col=0; col<num_vehicles; col++) {
		tableTexts[pair<int,int>(0, col+1)]->setText(vnames[col]);
	}

	root()->addWidget(container_);
}

void StatusViewApplication::update() {
	boost::mutex::scoped_lock lock(data_mutex);
	if (data.size()!=current_num_vehicles) {
		current_num_vehicles = data.size();
		reDraw(current_num_vehicles);
	}

	reDraw(current_num_vehicles);

	for (int col=0; col<current_num_vehicles; col++) {
		string vname = vnames[col];
		std::stringstream ss;

		ss << fixed << setprecision(1);
		ss << data[vname].voltage();
		tableTexts[pair<int,int>(VOLTAGE_ROW, col+1)]->setText(ss.str());

		ss.str("");
		ss << fixed << setprecision(2);
		ss << (MOOSTime()-data[vname].time());
		tableTexts[pair<int,int>(AGE_ROW, col+1)]->setText(ss.str());
	}
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
//				report_ages[pnr.name()] = MOOSTime()-pnr.time();
				if (find(vnames.begin(), vnames.end(), pnr.name())==vnames.end())
					vnames.push_back(pnr.name());
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
