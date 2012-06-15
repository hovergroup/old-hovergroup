/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J. Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Pulse.cpp                               */
/*    DATE: July 1st 2008  (For purposes of simple illustration) */
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
#include <math.h>
#include "BHV_Pulse.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "OF_Reflector.h"
#include "AOF_SimpleWaypoint.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Pulse::BHV_Pulse(IvPDomain gdomain) :
  IvPBehavior(gdomain)
{
  IvPBehavior::setParam("name", "simple_waypoint");
  m_domain = subDomain(m_domain, "course,speed");

  // All distances are in meters, all speed in meters per second
  // Default values for configuration parameters 
  m_desired_speed  = 0; 
  m_arrival_radius = 10;
  m_ipf_type       = "zaic";

  // Default values for behavior state variables
  m_osx  = 0;
  m_osy  = 0;
  m_curr_time = 0;
  m_wpt_index = -1;
  m_range = 10;
  m_pulse_duration = 1;
  did_pulse = true;

  addInfoVars("NAV_X, NAV_Y, WPT_INDEX");
}

//---------------------------------------------------------------
// Procedure: setParam - handle behavior configuration parameters

bool BHV_Pulse::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  double double_val = atof(val.c_str());
  if((param == "pulse_range")  && (isNumber(val))) {
    m_range = (int) double_val;
    return(true);
  }
  else if((param == "pulse_duration") && (isNumber(val))) {
    m_pulse_duration = (int) double_val;
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Pulse::onIdleState()
{
  postViewPoint(false);
}

//-----------------------------------------------------------
// Procedure: postViewPoint

void BHV_Pulse::postViewPoint(bool viewable)
{
  m_nextpt.set_label(m_us_name + "'s next waypoint");
  m_nextpt.set_type("waypoint");
  m_nextpt.set_source(m_descriptor);
  
  string point_spec;
  if(viewable)
    point_spec = m_nextpt.get_spec("active=true");
  else
    point_spec = m_nextpt.get_spec("active=false");
  postMessage("VIEW_POINT", point_spec);
}

void BHV_Pulse::postRangePulse() {
	XYRangePulse pulse;
	pulse.set_x(m_osx);
	pulse.set_y(m_osy);
	pulse.set_label("bhv_pulse");
	pulse.set_rad(m_range);
	pulse.set_duration(m_pulse_duration);
	pulse.set_time(m_curr_time);
	pulse.set_color("edge", "yellow");
	pulse.set_color("fill", "yellow");

	string spec = pulse.get_spec();
	postMessage("VIEW_RANGE_PULSE", spec);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Pulse::onRunState()
{
  // Part 1: Get vehicle position from InfoBuffer and post a 
  // warning if problem is encountered
  bool ok1, ok2, ok3;
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_curr_time = getBufferCurrTime();
  int new_waypoint_index = (int) getBufferDoubleVal("WPT_INDEX", ok3);
  if ( new_waypoint_index != m_wpt_index ) {
	  m_pulse_time = m_curr_time + 5;
	  m_wpt_index = new_waypoint_index;
	  did_pulse = false;
  }
  if ( m_curr_time > m_pulse_time && !did_pulse ) {
	  postRangePulse();
	  did_pulse = true;
  }
  if(!ok1 || !ok2) {
    postWMessage("No ownship X/Y info in info_buffer.");
    return(0);
  }
  
  // Part 2: Determine if the vehicle has reached the destination 
  // point and if so, declare completion.
//#ifdef WIN32
//  double dist = _hypot((m_nextpt.x()-m_osx), (m_nextpt.y()-m_osy));
//#else
//  double dist = hypot((m_nextpt.x()-m_osx), (m_nextpt.y()-m_osy));
//#endif
//  if(dist <= m_arrival_radius) {
//    setComplete();
//    postViewPoint(false);
//    return(0);
//  }
//
//  // Part 3: Post the waypoint as a string for consumption by
//  // a viewer application.
//  postViewPoint(true);
//
//  // Part 4: Build the IvP function with either the ZAIC tool
//  // or the Reflector tool.
//  IvPFunction *ipf = 0;
//  if(m_ipf_type == "zaic")
//    ipf = buildFunctionWithZAIC();
//  else
//    ipf = buildFunctionWithReflector();
//  if(ipf == 0)
//    postWMessage("Problem Creating the IvP Function");
  
  IvPFunction *ipf = 0;
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: buildFunctionWithZAIC

IvPFunction *BHV_Pulse::buildFunctionWithZAIC()
{
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(m_desired_speed);
  spd_zaic.setPeakWidth(0.5);
  spd_zaic.setBaseWidth(1.0);
  spd_zaic.setSummitDelta(0.8);  
  if(spd_zaic.stateOK() == false) {
    string warnings = "Speed ZAIC problems " + spd_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }
  
  double rel_ang_to_wpt = relAng(m_osx, m_osy, m_nextpt.x(), m_nextpt.y());
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(rel_ang_to_wpt);
  crs_zaic.setPeakWidth(0);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setSummitDelta(0);  
  crs_zaic.setValueWrap(true);
  if(crs_zaic.stateOK() == false) {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }

  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();

  OF_Coupler coupler;
  IvPFunction *ivp_function = coupler.couple(crs_ipf, spd_ipf, 50, 50);

  return(ivp_function);
}

//-----------------------------------------------------------
// Procedure: buildFunctionWithReflector

IvPFunction *BHV_Pulse::buildFunctionWithReflector()
{
  IvPFunction *ivp_function;

  bool ok = true;
  AOF_SimpleWaypoint aof_wpt(m_domain);
  ok = ok && aof_wpt.setParam("desired_speed", m_desired_speed);
  ok = ok && aof_wpt.setParam("osx", m_osx);
  ok = ok && aof_wpt.setParam("osy", m_osy);
  ok = ok && aof_wpt.setParam("ptx", m_nextpt.x());
  ok = ok && aof_wpt.setParam("pty", m_nextpt.y());
  ok = ok && aof_wpt.initialize();
  if(ok) {
    OF_Reflector reflector(&aof_wpt);
    reflector.create(600, 500);
    ivp_function = reflector.extractIvPFunction();
  }

  return(ivp_function);
}
