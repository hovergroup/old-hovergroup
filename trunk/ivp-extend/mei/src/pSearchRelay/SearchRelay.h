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
#include <sstream>
#include <iostream>
#include "XYSegList.h"
#include <string.h>
#include <math.h>
#include <lib_acomms_messages/acomms_messages.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

class SearchRelay : public CMOOSApp
{
public:
	SearchRelay();
	virtual ~SearchRelay();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

	void sendString(string, string, string);
	void sendDouble(string, string, double);

	void ComputeIndex(int);
	int Decision();
	void GetWaypoints();
	unsigned int closest_vertex(double, double);
	void ComputeSuccessRates(int);

protected:

	class RelayStat{
	public:
			string debug_string;
			int point_index;
			double stat_mean,stat_std, gittins_index;
			double stat_x,stat_y;
			double successful_packets;
			double num_obs;

			RelayStat(): debug_string("Default"), point_index(-1),
					stat_mean(-1), stat_std(-1), gittins_index(-1), stat_x(-1), stat_y(-1),
					successful_packets(-1), num_obs(0) {}
		};

	void Confess(RelayStat stats);

	string mode, mail, my_name;
	int discount,total_points,num_lookback, rate;
	map<double, vector<double> > data;
	vector<double> mean, stdev,indices;
	vector<double> normal_indices;
	vector<double> wpx, wpy;
	double fudge_factor, epsilon, station_factor;
	XYSegList seglist;
	double myx,myy;
	double targetx,targety;
	double wait_time, start_time, update_time, last_update;
	int connected;
	double mythrust, voltage, end_thrust;
	string end_status, relay_mode;
	double length;
	double first_obs, last_report, report_age;

	string action;

};

#endif
