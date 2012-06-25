/************************************************************/
/*    NAME: Mei Cheung                                      */
/*    ORGN: MIT                                             */
/*    FILE: SearchRelay.h                                   */
/*    DATE:                                                 */
/************************************************************/

#ifndef SearchRelay_HEADER
#define SearchRelay_HEADER

#include "MOOSLib.h"
#include <map>
#include <gsl/gsl_statistics_double.h>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <iostream>
#include "XYSegList.h"
#include <math.h>
#include <string.h>
#include <lib_acomms_messages/acomms_messages.h>

//namespace pt = boost::posix_time;

class SearchRelay : public CMOOSApp
{
public:
	SearchRelay();
	virtual ~SearchRelay();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void ComputeIndex(int);
	int Decision();
	//void UpdateStats(double);
	void GetWaypoints();
	std::string getRandomString(int);
	unsigned int closest_vertex(double, double);
	void ComputeSuccessRates(bool);

protected:

	class RelayStat{
	public:
			std::string debug_string;
			double x,y,next_x,next_y;
			double stat_mean,stat_std, index;
			double successful_packets;

			RelayStat(): debug_string("Default"), x(0), y(0), next_x(0), next_y(0),
					stat_mean(-1), stat_std(-1), index(-1), successful_packets(-1) {}
		};

	void Confess(RelayStat stats);

	// insert local vars here
	//all
	std::string my_role;
	int relay_id;

	//relay
	std::string mode,relay_message;
	int discount,min_obs,total_points,num_lookback;
	int cumulative_reward;
	std::map<double, std::vector<double> > data;
	std::vector<double> mean, stdev,indices;
	std::vector<double> normal_indices;
	std::vector<double> wpx, wpy;
	double fudge_factor, epsilon;
	XYSegList seglist;
	double myx,myy;
	double targetx,targety;
	bool waiting, relaying, relay_successful;

	//shore
	double last, wait_time;
	int rate,counter;
	std::string relay_status,end_status;
	std::string acomms_driver_status, start;
	//end
};

#endif
