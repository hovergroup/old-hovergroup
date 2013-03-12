//#include <QtGui/QApplication>
//#include <QtGui/QMainWindow>
//#include <QtGui/QScrollArea>
//#include <mgl2/qmathgl.h>

#include "HoverAcomms.h"
#include "goby/acomms/modem_driver.h"
#include "goby/acomms/protobuf/mm_driver.pb.h"

//int sample(mglGraph *gr)
//{
//  gr->Rotate(60,40);
//  gr->Box();
//  return 0;
//}

int main(int argc,char **argv)
{
	goby::acomms::protobuf::ModemTransmission proto;
	HoverAcomms::AcommsTransmission trans;
	trans.setDest(0);
	std::cout << trans.serialize() << std::endl;
	std::string s = trans.serialize();
	for (int i=0; i<s.size(); i++) {
		std::cout << std::hex << (int) s[i] << ":";
	}
	std::cout << std::endl;

	std::cout << trans.parseFromString(s) << std::endl;
	std::cout << trans.getRate() << std::endl;
	trans.setRate(trans.getRate());
	std::cout << trans.getRate() << std::endl;


	HoverAcomms::AcommsReception reception;
	reception.copyFromProtobuf(proto);
	std::cout << reception.m_protobuf.DebugString() << std::endl;
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
