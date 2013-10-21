/*
 * JoshUtil.h
 *
 *  Created on: Nov 11, 2012
 *      Author: josh
 */

#ifndef LIB_JOSHUTIL_H_
#define LIB_JOSHUTIL_H_

#include "LogUtils.h"
#include "MBUtils.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <boost/filesystem.hpp>
#include <exception>
#include <algorithm>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "config.h"

namespace JoshUtil {
	ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings = false);
	bool wildCardMatch( std::string wild, std::string key );

	void searchForFiles( std::vector<std::string> & paths,
			std::string directory_path,
			int max_depth,
			std::string wild );

	double getSystemTimeSeconds();

	/** Get a current slot using time, period, and optional base_offset. */
	double getSlot(double time, double period, double offset);

	/** Get current slot using current time, period, and optional base_offset. */
	double getSlot(double period, double offset);

};

#endif /* LIB_JOSHUTIL_H_ */
