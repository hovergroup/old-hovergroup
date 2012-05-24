// $Header: /home/cvsroot/project-marine-shell/src/iOS5000/CiOS5000.h,v 1.4 2007/08/03 19:58:50 anrp Exp $
// (c) 2004 
// CiOS5000.h: interface for the CiOS5000 class.
////////////////////////////////////////////////

#ifndef __CiOS5000_filtered_h__
#define __CiOS5000_filtered_h__

#include "MOOSLib.h"
#include "CSerialPort.h"

class CiOS5000_filtered : public CMOOSApp
{
public:
	CiOS5000_filtered();
	virtual ~CiOS5000_filtered();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	CSerialPort *pt;

	double prerotation, current_x_estimate, current_y_estimate, update_fraction;

	pthread_t thr;
	static void *trampoline(void *arg) {
		((CiOS5000_filtered *)arg)->thread();
		return NULL;
	}

	void thread(void);
};

#endif /* __CiOS5000_h__ */
