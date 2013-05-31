//#include <QtGui/QApplication>
//#include <QtGui/QMainWindow>
//#include <QtGui/QScrollArea>
//#include <mgl2/qmathgl.h>

//#include "HoverAcomms.h"
//#include "goby/acomms/modem_driver.h"
//#include "goby/acomms/protobuf/mm_driver.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>

//#include <iostream>
//int sample(mglGraph *gr)
//{
//  gr->Rotate(60,40);
//  gr->Box();
//  return 0;
//}

int main(int argc,char **argv)
{

	std::string date = "2013/05/31 15:48:07.020";

	boost::posix_time::ptime t(boost::posix_time::time_from_string(date));

	std::cout << t << std::endl;


	std::string line = "2013/05/31 15:48:07.000,  42.358579515, -71.087503447,  -23.0271,  5,  5, 17.2777,  5.2547, 14.0589, -7.9344, -6.1982, 13.4823,  0.00,   0.0";




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
}
