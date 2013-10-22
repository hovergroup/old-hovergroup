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


	// Slot functions:
	class SlotFunctions {
	public:
		SlotFunctions() { base_offset = 0; }

		double period, base_offset;

		// get the previous/current slot
		int getLastSlot(double time) { return floor((time-base_offset)/period); }
		int getLastSlot() { return getLastSlot(getSystemTimeSeconds()); }

		// get fractional position inside a slot
		double getSlotFraction(double time) { double d; return modf((time-base_offset)/period, &d); }
		double getSlotFraction() { return getSlotFraction(getSystemTimeSeconds()); }

		// get seconds into a slot
		double getSlotFractionSeconds(double time) { return getSlotFraction(time)*period; }
		double getSlotFractionSeconds() { return getSlotFractionSeconds(getSystemTimeSeconds()); }

		// get the next slot
		int getNextSlot(double time) { return ceil((time-base_offset)/period); }
		int getNextSlot() { return getNextSlot(getSystemTimeSeconds()); }

		// convert slot to system time
		double slot2seconds(int slot) { return slot*period + base_offset; }
	};
};

#endif /* LIB_JOSHUTIL_H_ */
