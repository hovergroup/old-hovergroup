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
			double successful_packets;

			RelayStat(): debug_string("Default"), point_index(-1),
					stat_mean(-1), stat_std(-1), gittins_index(-1), successful_packets(-1) {}
		};

	void Confess(RelayStat stats);

	string mode, mail;
	int discount,min_obs,total_points,num_lookback, rate;
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
	double mythrust, voltage;
	string end_status, relay_mode;

	string action;

};

#endif
