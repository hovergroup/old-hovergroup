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

namespace JoshUtil {
	ALogEntry getNextRawALogEntry_josh(FILE *fileptr, bool allstrings);
	bool wildCardMatch( std::string wild, std::string key );

	void searchForFiles( std::vector<std::string> & paths,
			std::string directory_path,
			int max_depth,
			std::string wild );
};

#endif /* LIB_JOSHUTIL_H_ */
