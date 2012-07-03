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
	void ComputeSuccessRates(bool);

protected:

	class RelayStat{
	public:
			string debug_string;
			double x,y,next_x,next_y;
			double stat_mean,stat_std, index;
			double successful_packets;

			RelayStat(): debug_string("Default"), x(0), y(0), next_x(0), next_y(0),
					stat_mean(-1), stat_std(-1), index(-1), successful_packets(-1) {}
		};

	void Confess(RelayStat stats);

	//relay
	string mode;
	int discount,min_obs,total_points,num_lookback, rate;
	map<double, std::vector<double> > data;
	vector<double> mean, stdev,indices;
	vector<double> normal_indices;
	vector<double> wpx, wpy;
	double fudge_factor, epsilon;
	XYSegList seglist;
	double myx,myy;
	double targetx,targety;
	bool waiting, relaying;
	int connected;

};

#endif
