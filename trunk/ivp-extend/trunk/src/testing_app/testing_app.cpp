//#include <QtGui/QApplication>
//#include <QtGui/QMainWindow>
//#include <QtGui/QScrollArea>
//#include <mgl2/qmathgl.h>

//#include "HoverAcomms.h"
//#include "goby/acomms/modem_driver.h"
//#include "goby/acomms/protobuf/mm_driver.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "HelmReportUtils.h"
#include "MBUtils.h"
#include <vector>
#include <string>

using namespace std;

int main(int argc,char **argv)
{

	//string input = "iter=45,utc_time=1379176756.94,ofnum=2,var=speed:2,var=course:163,active_bhvs=goto_and_station$1379176756.94$100.00000$9$0.01000$1/1$1:goto_and_return$1379176756.94$100.00000$9$0.01000$1/1$1,idle_bhvs=return$0.00$n/a:Archie_Stationkeep$1379176756.94$n/a";
	string input = "iter=45,utc_time=1379176756.94,ofnum=2,var=speed:2,var=course:163,active_bhvs=goto_and_station$1379176756.94$100.00000$9$0.01000$1/1$1,idle_bhvs=return$0.00$n/a:Archie_Stationkeep$1379176756.94$n/a";

	vector<string> svector = parseStringQ(input, ',');
	unsigned int i, vsize = svector.size();
	for (i = 0; i < vsize; i++) {
		string left = biteStringX(svector[i], '=');
		string right = svector[i];

		if (left == "active_bhvs") {
			std::string actives = biteStringX(right, '$');

			while (right.find(":")!=string::npos) {
				biteStringX(right, ':');
				actives += (", " + biteStringX(right, '$'));
			}
			std::cout << actives << endl;
		}

	}

	return 0;
}



	//  QApplication a(argc,argv);
//  QMainWindow *Wnd = new QMainWindow;
//  Wnd->resize(810,610);  // for fill up the QMGL, menu and toolbars
//  Wnd->setWindowTitle("QMathGL sample");
//  // here I allow to scroll QMathGL -- the case
//  // then user want to prepare huge picture
//  QScrollArea *scroll = new QScrollArea(Wnd);
//
//  // Create and setup QMathGL
//  QMathGL *QMGL = new QMathGL(Wnd);
////QMGL->setPopup(popup); // if you want to setup popup menu for QMGL
//  QMGL->setDraw(sample);
//  // or use QMGL->setDraw(foo); for instance of class Foo:public mglDraw
//  QMGL->update();
//
//  // continue other setup (menu, toolbar and so on)
//  scroll->setWidget(QMGL);
//  Wnd->setCentralWidget(scroll);
//  Wnd->show();
//  return a.exec();
