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
#include <Wt/WCssStyleSheet>

using namespace Wt;
using namespace std;

boost::mutex data_mutex;
std::vector<std::string> vnames;
std::map<std::string, double> report_ages;
std::map<std::string, ProtoNodeReport> data;

const int NUM_ROWS = 10;
const int AGE_ROW = 1;
const int HEALTH_ROW = 2;
const int VOLTAGE_ROW = 3;
const int BATT_PERCENT_ROW = 4;
const int GPS_QUALITY_ROW = 5;
const int ACOMMS_DRIVER_STATUS_ROW = 6;
const int HELM_STATE_ROW = 7;
const int ACTIVE_BEHAVIORS_ROW = 8;
const int RADIO_STATE_ROW = 9;

//---------------------------------------------------------
// Constructor

StatusViewApplication::StatusViewApplication(const WEnvironment& env) :
	WApplication(env) {
	setTitle("Status View");

	styleSheet().addRule(".StatusView table, td, th", "border: 2px solid #DDD; width: 0%; margin-top: 20px; margin-bottom: 20px; padding: 4px 5px;");
	styleSheet().addRule(".StatusView .table-bordered", "border-left: 4px; border-radius: 4px;");
	styleSheet().addRule(".StatusView .col0", "font-weight: bold; text-align: left");
	styleSheet().addRule(".StatusView .center", "text-align: center");
	styleSheet().addRule(".StatusView .green", "background-color: #58FA58;");
	styleSheet().addRule(".StatusView .yellow", "background-color: #F7FE2E;");
	styleSheet().addRule(".StatusView .red", "background-color: #FE2E2E;");

	reDraw(0);
	current_num_vehicles = -1;
	iterations = 0;

	WTimer *timer = new WTimer();
	timer->setInterval(1000);
	timer->timeout().connect(this, &StatusViewApplication::update);
	timer->start();
}

void StatusViewApplication::reDraw(int num_vehicles) {
	root()->clear();
	tableTexts.clear();

	container_ = new WContainerWidget();
	container_->setStyleClass("StatusView");
	table = new WTable(container_);
	table->setHeaderCount(1);
	table->setStyleClass("table table-bordered");
    table->columnAt(0)->setWidth(75);
    for (int i=1; i<table->columnCount(); i++) {
    	table->columnAt(i)->setWidth(150);
    }

	for (int column=0; column<num_vehicles+1; column++) {
		for (int row=0; row<NUM_ROWS; row++) {
			WText *t = new WText();
			if (column==0 && row!=0)
				t->setStyleClass("col0");
			else if (column!=0)
				t->setStyleClass("center");
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
	tableTexts[pair<int,int>(ACOMMS_DRIVER_STATUS_ROW,0)]->setText("Acomms Status");
	tableTexts[pair<int,int>(HELM_STATE_ROW,0)]->setText("Helm State");
	tableTexts[pair<int,int>(ACTIVE_BEHAVIORS_ROW,0)]->setText("Active Behaviors");
	tableTexts[pair<int,int>(RADIO_STATE_ROW,0)]->setText("Radio Mode");

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

//	reDraw(current_num_vehicles);

	std::stringstream ss;
	ss << iterations;
	tableTexts[pair<int,int>(0, 0)]->setText(ss.str());
	iterations++;

	// loop over all vehicles
	for (int col=0; col<current_num_vehicles; col++) {
		string vname = vnames[col];

		// ---------------- voltage -------------------
		if (data[vname].has_nsf_power()) { // for nsf nodes
			if (data[vname].nsf_power() == ProtoNodeReport_NSFPowerEnum_OKAY) {
				tableTexts[pair<int,int>(VOLTAGE_ROW, col+1)]->setText("okay");
			    table->elementAt(VOLTAGE_ROW, col+1)->setStyleClass("green center");
			} else {
				tableTexts[pair<int,int>(VOLTAGE_ROW, col+1)]->setText("low");
	            table->elementAt(VOLTAGE_ROW, col+1)->setStyleClass("yellow center");
			}
		} else { // for normal vehicles
			ss.str("");
			ss << fixed << setprecision(1);
			double volts = data[vname].voltage();
			ss << volts;
			tableTexts[pair<int,int>(VOLTAGE_ROW, col+1)]->setText(ss.str());
			if (volts >= 12.2)
				table->elementAt(VOLTAGE_ROW, col+1)->setStyleClass("green center");
			else if (volts >= 11.7)
				table->elementAt(VOLTAGE_ROW, col+1)->setStyleClass("yellow center");
			else
				table->elementAt(VOLTAGE_ROW, col+1)->setStyleClass("red center");
		}


		// ---------------- age -------------------
		ss.str("");
		double age = (MOOSTime()-data[vname].time_stamp());
		ss << fixed << setprecision(2) << age;
		tableTexts[pair<int,int>(AGE_ROW, col+1)]->setText(ss.str());
		if (age < 3)
			table->elementAt(AGE_ROW, col+1)->setStyleClass("green center");
		else if (age > 10)
			table->elementAt(AGE_ROW, col+1)->setStyleClass("red center");
		else
			table->elementAt(AGE_ROW, col+1)->setStyleClass("yellow center");

		// ---------------- helm state -------------------
		string helm_state;
		switch(data[vname].helm_state()) {
		case ProtoNodeReport_HelmStateEnum_DRIVE:
			helm_state = "DRIVE";
			table->elementAt(HELM_STATE_ROW, col+1)->setStyleClass("green center");
			break;
		case ProtoNodeReport_HelmStateEnum_PARK:
			helm_state = "PARK";
			table->elementAt(HELM_STATE_ROW, col+1)->setStyleClass("yellow center");
			break;
		case ProtoNodeReport_HelmStateEnum_MISSING:
			helm_state = "MISSING";
			table->elementAt(HELM_STATE_ROW, col+1)->setStyleClass("red center");
			break;
		default:
			helm_state = "";
			break;
		}
		tableTexts[pair<int,int>(HELM_STATE_ROW, col+1)]->setText(helm_state);

		// ---------------- active behaviors -------------------
		ss.str("");
		for (int i=0; i<data[vname].active_behaviors_size(); i++) {
			ss << data[vname].active_behaviors(i);
			if (i<data[vname].active_behaviors_size()-1)
				ss << endl;
		}
		tableTexts[pair<int,int>(ACTIVE_BEHAVIORS_ROW, col+1)]->setText(ss.str());
		table->elementAt(ACTIVE_BEHAVIORS_ROW, col+1)->setStyleClass("center");

		// ---------------- acomms driver status -------------------
		string acomms_running;
		switch(data[vname].acomms_status()) {
		case ProtoNodeReport_AcommsStatusEnum_READY:
			acomms_running = "ready";
			table->elementAt(ACOMMS_DRIVER_STATUS_ROW, col+1)->setStyleClass("green center");
			break;
		case ProtoNodeReport_AcommsStatusEnum_TRANSMITTING:
			acomms_running = "transmitting";
			table->elementAt(ACOMMS_DRIVER_STATUS_ROW, col+1)->setStyleClass("green center");
			break;
		case ProtoNodeReport_AcommsStatusEnum_RECEIVING:
			acomms_running = "receiving";
			table->elementAt(ACOMMS_DRIVER_STATUS_ROW, col+1)->setStyleClass("green center");
			break;
		case ProtoNodeReport_AcommsStatusEnum_NOT_RUNNING:
			acomms_running = "not running";
			table->elementAt(ACOMMS_DRIVER_STATUS_ROW, col+1)->setStyleClass("yellow center");
			break;
		case ProtoNodeReport_AcommsStatusEnum_OFFLINE:
			acomms_running = "offline";
			table->elementAt(ACOMMS_DRIVER_STATUS_ROW, col+1)->setStyleClass("red center");
			break;
		default:
			break;
		}
		tableTexts[pair<int,int>(ACOMMS_DRIVER_STATUS_ROW, col+1)]->setText(acomms_running);

		// ---------------- gps quality -------------------
		string gps_quality;
		switch(data[vname].gps_quality()) {
		case ProtoNodeReport_GPSQualityEnum_FIX:
			gps_quality = "rtk fix";
			table->elementAt(GPS_QUALITY_ROW, col+1)->setStyleClass("green center");
			break;
		case ProtoNodeReport_GPSQualityEnum_FLOAT:
			gps_quality = "rtk float";
			table->elementAt(GPS_QUALITY_ROW, col+1)->setStyleClass("yellow center");
			break;
		case ProtoNodeReport_GPSQualityEnum_SINGLE:
			gps_quality = "rtk single";
			table->elementAt(GPS_QUALITY_ROW, col+1)->setStyleClass("yellow center");
			break;
		case ProtoNodeReport_GPSQualityEnum_INTERNAL:
			gps_quality = "internal";
			table->elementAt(GPS_QUALITY_ROW, col+1)->setStyleClass("yellow center");
			break;
		case ProtoNodeReport_GPSQualityEnum_NO_GPS:
			gps_quality = "unavailable";
			table->elementAt(GPS_QUALITY_ROW, col+1)->setStyleClass("red center");
			break;
		case ProtoNodeReport_GPSQualityEnum_NO_MANAGER:
			gps_quality = "no manager";
			table->elementAt(GPS_QUALITY_ROW, col+1)->setStyleClass("red center");
			break;
		default:
			break;
		}
		tableTexts[pair<int,int>(GPS_QUALITY_ROW, col+1)]->setText(gps_quality);

		// ---------------- radio mode -------------------
		string radio_mode;
		switch(data[vname].radio_state()) {
		case ProtoNodeReport_RadioStateEnum_BULLET_LOCKED:
			radio_mode = "bullet";
			table->elementAt(RADIO_STATE_ROW, col+1)->setStyleClass("green center");
			break;
		case ProtoNodeReport_RadioStateEnum_BULLET_UNLOCKED:
			radio_mode = "bullet";
			table->elementAt(RADIO_STATE_ROW, col+1)->setStyleClass("yellow center");
			break;
		case ProtoNodeReport_RadioStateEnum_FREEWAVE_LOCKED:
			radio_mode = "freewave";
			table->elementAt(RADIO_STATE_ROW, col+1)->setStyleClass("green center");
			break;
		case ProtoNodeReport_RadioStateEnum_FREEWAVE_UNLOCKED:
			radio_mode = "freewave";
			table->elementAt(RADIO_STATE_ROW, col+1)->setStyleClass("yellow center");
			break;
		default:
			break;
		}
		tableTexts[pair<int,int>(RADIO_STATE_ROW, col+1)]->setText(radio_mode);
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
				data[pnr.vehicle_name()] = pnr;
//				report_ages[pnr.name()] = MOOSTime()-pnr.time();
				if (find(vnames.begin(), vnames.end(), pnr.vehicle_name())==vnames.end())
					vnames.push_back(pnr.vehicle_name());
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
			"--http-address=0.0.0.0",
			"--http-port=8080"
	};

	WRun(4, myArgv, &createApplication);
}
