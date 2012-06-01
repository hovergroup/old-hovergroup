/*
 * acomms_alog_parser.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */

#include "acomms_alog_parser.h"

using namespace std;

ACOMMS_ALOG_PARSER::ACOMMS_ALOG_PARSER() {
	FILE *m_file_one = fopen("mylog.alog", "r");
    string line_raw = getNextRawLine(m_file_one);
}
