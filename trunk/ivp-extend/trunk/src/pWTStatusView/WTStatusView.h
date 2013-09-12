/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: WTStatusView.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef WTStatusView_HEADER
#define WTStatusView_HEADER

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WTable>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTimer>
#include "MOOS/libMOOS/MOOSLib.h"
#include "reports.pb.h"
#include <boost/thread.hpp>

class WTStatusView: public CMOOSApp {
public:
	WTStatusView();
	~WTStatusView();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	void startWT();

private:
	boost::thread wtThread;

};

class StatusViewApplication: public Wt::WApplication {
public:
	StatusViewApplication(const Wt::WEnvironment& env);

private:
	void reDraw(int num_vehicles);
	void update();

	Wt::WText *voltage_;

	int current_num_vehicles;

private:
	// widgets
	Wt::WContainerWidget *container_;


};

#endif 
