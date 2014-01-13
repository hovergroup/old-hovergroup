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

#include "MOOS/libMOOS/Comms/MOOSAsyncCommClient.h"

MOOS::MOOSAsyncCommClient sim_Comms;

using namespace std;

bool OnConnect(void * pParam) {
    cout << "Registering for MY_VAR" << endl;
    sim_Comms.Register("MY_VAR", 0);
    return true;
}

bool OnMail(void *pParam) {
    MOOSMSG_LIST M;
    sim_Comms.Fetch(M);
    MOOSMSG_LIST::iterator q;
    for (q=M.begin(); q!=M.end(); q++) {
        q->Trace();
    }
    return true;
}

int main(int argc,char **argv)
{
    sim_Comms.SetOnConnectCallBack(OnConnect, &sim_Comms);
    sim_Comms.SetOnMailCallBack(OnMail, &sim_Comms);
    sim_Comms.Run("localhost", 9000, "appname");

    while (true) {
        MOOSPause(1000);
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
