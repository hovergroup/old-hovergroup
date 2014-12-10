/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Witsen.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Witsen_HEADER
#define Witsen_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdio.h>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

class Witsen : public CMOOSApp
{
public:
	Witsen();
	virtual ~Witsen();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	int id;
	double nav_x, nav_y;
	double sim_x, sim_y;
	double sim_origin_x, sim_origin_y;
	double standoff1, standoff2;
	double targ_x, targ_y;
	double x, u;
	double x_desired, y_desired;
	double delta, sigma1, sigma2, sim_step, max_dev;
	std::string command;
	boost::variate_generator<boost::mt19937, boost::normal_distribution<> > generator;

};

#endif 
