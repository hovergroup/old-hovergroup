/*
 * TimedAcomms.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: josh
 */

#include "TargetSim.h"
#include "math.h"

std::pair<double, double> BaseSim::getTargetPos() {
	return std::pair<double, double>(m_osx, m_osy);
}

void BaseSim::getTargetPos(double & x, double & y) {
	x = m_osx;
	y = m_osy;
}

void CircleSim::setConfiguration(CProcessConfigReader & m_MissionReader) {
	m_MissionReader.GetConfigurationParam("center_x", m_centerX);
	m_MissionReader.GetConfigurationParam("center_y", m_centerY);
	m_MissionReader.GetConfigurationParam("radius", m_radius);
	double speed;
	m_MissionReader.GetConfigurationParam("speed", speed);
	m_angularSpeed = speed/m_radius;
	m_MissionReader.GetConfigurationParam("initial_angle", m_initial_angle);
	m_MissionReader.GetConfigurationParam("clockwise", m_clockwise);

	initialPos();
}

void CircleSim::reset() {
	m_state = Paused;
	initialPos();
}

void CircleSim::pause() {
	m_state = Paused;
}

void CircleSim::resume() {
	m_state = Running;
}

void CircleSim::initialPos() {
	setPos(m_initial_angle);
}

void CircleSim::setPos(double angle) {
	m_osx = m_centerX + m_radius*sin(angle*M_PI/180.0);
	m_osy = m_centerY + m_radius*cos(angle*M_PI/180.0);
}

void CircleSim::doWork(double time) {
	m_lastRunTime = m_thisRunTime;
	m_thisRunTime = time;

	if ( m_state == Paused ) return;

	double timeDiff = m_thisRunTime - m_lastRunTime;
	double angleDiff = timeDiff * m_angularSpeed;
	if (m_clockwise)
		m_currentAngle += angleDiff;
	else
		m_currentAngle -= angleDiff;
	while ( m_currentAngle > 360 ) m_currentAngle -= 360.0;
	while ( m_currentAngle < -360 ) m_currentAngle += 360.0;

	setPos( m_currentAngle );
}
