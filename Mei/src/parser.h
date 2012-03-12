#ifndef __PARSER_H
#define __PARSER_H

#include <boost/assign.hpp>

class PARSER {
	
	public: 
		PARSER();
	
	private:
		enum TalkerIDs {TALKER_NOT_DEFINED = 0,CA,CC,SN,GP};

		enum SentenceIDs  { SENTENCE_NOT_DEFINED = 0,
							ACK,DRQ,RXA,RXD,
							RXP,TXD,TXA,TXP,
							TXF,CYC,MPC,MPA,
							MPR,RSP,MSC,MSA,
							MSR,EXL,MEC,MEA,
							MER,MUC,MUA,MUR,
							PDT,PNT,TTA,MFD,
							CLK,CFG,AGC,BBD,
							CFR,CST,MSG,REV,
							DQF,SHF,SNR,DOP,
							DBG,FFL,FST,ERR,
							TOA,XST};
		
		std::map<std::string, TalkerIDs> talker_id_map_;
		std::map<std::string, SentenceIDs> sentence_id_map_;
		std::map<std::string, std::string> description_map_;
		std::map<std::string, std::string> cfg_map_;
		
		void initialize_maps();
}
