/*
 * acomms_alog_paser.h
 *
 *  Created on: Jun 1, 2012
 *      Author: josh
 */


#include "goby/acomms/protobuf/mm_driver.pb.h"
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include "LogUtils.h"
#include <stdio.h>
#include <vector>

#ifndef ACOMMS_ALOG_PARSER_H_
#define ACOMMS_ALOG_PARSER_H_


class ACOMMS_ALOG_PARSER {
public:
	ACOMMS_ALOG_PARSER();
	~ACOMMS_ALOG_PARSER() {};

	void addAlogFile( std::string filename );
	void addAlogFile( boost::filesystem::path filepath );

	void runParser();

	class FILE_INFO {
	public:
		FILE_INFO() {}

		std::string filename;
		FILE * logfile;
		std::vector<std::string> header_lines;

		std::string getNextLine();
		void parseHeaderLines();
	};

private:
	void parseAllHeaders();

	std::vector<FILE_INFO> alog_files;

};


#endif /* ACOMMS_ALOG_PARSER_H_ */
