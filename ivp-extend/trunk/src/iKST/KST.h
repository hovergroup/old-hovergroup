/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: KST.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef KST_HEADER
#define KST_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <math.h>

class KST: public CMOOSApp {
public:
	KST();
	~KST();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void RegisterVariables();

private:
	static const char delim = ',';

	std::string m_outputFilePath;

	std::vector<std::string> m_vars;
	std::map<std::string,double> m_values;

	double m_startTime;
	int m_allocated_columns;

	std::ofstream out;

	void printHeader();
	void printLine();

};

#endif 
