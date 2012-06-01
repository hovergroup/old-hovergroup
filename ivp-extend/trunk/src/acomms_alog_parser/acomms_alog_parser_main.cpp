/*
 * acomms_alog_parser_main.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */


#include "goby/acomms/protobuf/mm_driver.pb.h"
#include <google/protobuf/text_format.h>
#include "MBUtils.h"
#include "LogUtils.h"
#include <stdio.h>

using namespace std;

int main () {
	FILE *m_file_one = fopen("mylog.alog", "r");
    string line_raw = getNextRawLine(m_file_one);

	return 0;
}
