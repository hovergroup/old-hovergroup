/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SearchRelay.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef SearchRelay_HEADER
#define SearchRelay_HEADER

#include "MOOSLib.h"
#include <map>
#include <gsl/gsl_statistics_double.h>
#include <acomms_messages.h>

class SearchRelay : public CMOOSApp
{
public:
	SearchRelay();
	virtual ~SearchRelay();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void UpdateStats(std::vector<double>);

protected:
	// insert local vars here
	lib_acomms_messages::LOSS_RATE_INFO loss_rates;
	int min_packets;
	double mean, var;

	bool decision;
	std::string my_role;
	std::vector<double> data;
};

#endif 
