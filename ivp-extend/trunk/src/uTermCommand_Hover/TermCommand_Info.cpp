/*
 * uTermCommand_Hover
 *        File: TermCommand_Info.cpp
 *  Created on: May 19, 2014
 *      Author: Josh Leighton
 */

/*
 * This file is modified from the original MOOS-IvP version to
 * support poking of multiple variables via a single command.
 */

/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TermCommand_Info.cpp                                 */
/*    DATE: Aug 25th 2011                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <cstdlib>
#include <iostream>
#include "TermCommand_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis() {
    blk("SYNOPSIS:                                                       ");
    blk("------------------------------------                            ");
    blk("  The uTermCommand application is a terminal based tool for     ");
    blk("  poking the MOOS database with pre-defined variable-value      ");
    blk("  pairs. A unique key may be associated with each poke.         ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit() {
    blk("                                                                ");
    blu("=============================================================== ");
    blu("Usage: uTermCommandfile.moos [OPTIONS]                          ");
    blu("=============================================================== ");
    blk("                                                                ");
    showSynopsis();
    blk("                                                                ");
    blk("Options:                                                        ");
    mag("  --alias", "=<ProcessName>                                      ");
    blk("      Launch uTermCommand with the given process name rather    ");
    blk("      than uTermCommand                                         ");
    mag("  --example, -e                                                 ");
    blk("      Display example MOOS configuration block.                 ");
    mag("  --help, -h                                                    ");
    blk("      Display this help message.                                ");
    mag("  --interface, -i                                               ");
    blk("      Display MOOS publications and subscriptions.              ");
    mag("  --verbose", "=Boolean (true/false)                             ");
    blk("      Display diagnostics messages. Default is true.            ");
    mag("  --version,-v                                                  ");
    blk("      Display the release version of uTermCommand.              ");
    blk("                                                                ");
    blk("Note: If argv[2] does not otherwise match a known option,       ");
    blk("      then it will be interpreted as a run alias. This is       ");
    blk("      to support pAntler launching conventions.                 ");
    blk("                                                                ");
    exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit() {
    blk("                                                                ");
    blu("=============================================================== ");
    blu("uTermCommand Example MOOS Configuration                         ");
    blu("=============================================================== ");
    blk("                                                                ");
    blk("ProcessConfig = uTermCommand                                    ");
    blk("{                                                               ");
    blk("  AppTick   = 4                                                 ");
    blk("  CommsTick = 4                                                 ");
    blk("                                                                ");
    blk("  CMD  =  1 -->  DEPLOY -->               false                 ");
    blk("  CMD  =  2 -->  DEPLOY -->               false                 ");
    blk("  CMD  =  3 -->  MOOS_MANUAL_OVERIDE -->  true                  ");
    blk("  CMD  =  4 -->  MOOS_MANUAL_OVERIDE -->  false                 ");
    blk("  CMD  =  5 -->  STATION_KEEP -->         true                  ");
    blk("  CMD  =  6 -->  STATION_KEEP -->         false                 ");
    blk("  CMD  =  7 -->  RETURN -->               true                  ");
    blk("  CMD  =  8 -->  RETURN -->               false                 ");
    blk("                                                                ");
    blk("  CMD  =  px --> PERMUTATIONS -->         false                 ");
    blk("  CMD  =  pg --> PERMUTATIONS -->         true                  ");
    blk("  CMD  =  pr --> LOITER_REMAP -->         true                  ");
    blk("                                                                ");
    blk("  CMD  =  hv --> HELM_VERBOSE -->         verbose               ");
    blk("  CMD  =  ht --> HELM_VERBOSE -->         terse                 ");
    blk("  CMD  =  hq --> HELM_VERBOSE -->         quiet                 ");
    blk("}                                                               ");
    blk("                                                                ");
    exit(0);
}

//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit() {
    blk("                                                                ");
    blu("=============================================================== ");
    blu("uTermCommand INTERFACE                                          ");
    blu("=============================================================== ");
    blk("                                                                ");
    showSynopsis();
    blk("                                                                ");
    blk("SUBSCRIPTIONS:                                                  ");
    blk("------------------------------------                            ");
    blk("  None                                                          ");
    blk("                                                                ");
    blk("PUBLICATIONS:                                                   ");
    blk("------------------------------------                            ");
    blk("  User configured variable-value pairs.                         ");
    blk("                                                                ");
    exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit() {
    showReleaseInfo("uTermCommand", "gpl");
    exit(0);
}

