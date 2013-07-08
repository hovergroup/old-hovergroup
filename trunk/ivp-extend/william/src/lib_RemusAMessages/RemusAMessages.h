/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: RemusAMessages.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RemusAMessages_HEADER
#define RemusAMessages_HEADER

#include <string>

namespace RemusAMessages {
	class RemusStatusM {
	public:
		RemusStatusM() {};
		RemusStatusM( std::string msg );

		std::string vname;
		double nav_x, nav_y, nav_d, nav_b; //[x y depth,bearing]

		std::string toString();
	};

	class RemusCmdM {
	public:
		RemusCmdM() {};
		RemusCmdM( std::string msg );

		std::string cmd;

		std::string toString();
	};
}

#endif 
