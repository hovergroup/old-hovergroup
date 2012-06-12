/*
 * acomms_alog_parser_output.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: josh
 */


#include "acomms_alog_parser.h"

// parsed data is stored in the vectors matched_transmit_events and leftover_receive_events
//
// matched_transmit_events contains all transmission events and receipts that have been
// matched to them.  If a receive event was not found for a node, sync loss is assumed.
//
// leftover_receive_events contains all receive events that could not be matched to a
// transmission event.  This may indicate you are missing log files or simply an error in
// the matcher.
//
// A list of all vehicle names can be found in the vector vehicle_names.  The vehicle names
// are treated as their unique identifiers, not the vehicle IDs.
//
// Significant features not currently implemented:
//   FSK Mini only transmissions (rate -1)
//   Receive status other than sync loss (2)
//   Non-ASCII character transmissions

void ACOMMS_ALOG_PARSER::outputResults() {
	// YOUR CODE HERE
}
