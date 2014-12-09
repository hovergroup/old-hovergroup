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

// --------------------------------------------------------------

void CircleSim::setConfiguration(CProcessConfigReader & m_MissionReader) {
	m_MissionReader.GetConfigurationParam("center_x", m_centerX);
	m_MissionReader.GetConfigurationParam("center_y", m_centerY);
	m_MissionReader.GetConfigurationParam("radius", m_radius);
	double speed;
	m_MissionReader.GetConfigurationParam("speed", speed);
	m_angularSpeed = speed/m_radius * 180.0/M_PI;
	m_MissionReader.GetConfigurationParam("initial_angle", m_initial_angle);
	m_MissionReader.GetConfigurationParam("clockwise", m_clockwise);

//	std::cout << m_centerX << " ";
//	std::cout << m_centerY << " ";
//	std::cout << m_radius << " ";
//	std::cout << m_angularSpeed << " ";
//	std::cout << m_clockwise << std::endl;

	initialPos();

//	std::cout << m_osx << " " << m_osy << std::endl;
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
	m_currentAngle = m_initial_angle;
}

void CircleSim::setPos(double angle) {
	m_osx = m_centerX + m_radius*sin(angle*M_PI/180.0);
	m_osy = m_centerY + m_radius*cos(angle*M_PI/180.0);
}

void CircleSim::doWork(double time) {
	m_lastRunTime = m_thisRunTime;
	m_thisRunTime = time;

	if ( first_iteration ) {
		first_iteration = false;
		return;
	}

//	std::cout << m_currentAngle << std::endl;
//	std::cout << m_osx << " " << m_osy << std::endl;
	if ( m_state == Paused ) return;
//	std::cout << "not paused" << std::endl;

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

// --------------------------------------------------------------

void AcceleratingCircleSim::reset() {
	m_state = Paused;
	initialVel();
	initialPos();
}

void AcceleratingCircleSim::initialVel() {
	if ( m_acceleration > 0 )
		m_currentVelocity = m_minVelocity;
	else
		m_currentVelocity = m_maxVelocity;
}

/**
 * acceleration can be negative or positive, units of m/s^2
 * will start at min or max speed appropriately
 */
void AcceleratingCircleSim::setConfiguration(CProcessConfigReader & m_MissionReader) {
	m_MissionReader.GetConfigurationParam("center_x", m_centerX);
	m_MissionReader.GetConfigurationParam("center_y", m_centerY);
	m_MissionReader.GetConfigurationParam("radius", m_radius);
	double speed;
	m_MissionReader.GetConfigurationParam("min_speed", speed);
	m_minVelocity = speed/m_radius * 180.0/M_PI;
	m_MissionReader.GetConfigurationParam("max_speed", speed);
	m_maxVelocity = speed/m_radius * 180.0/M_PI;
	m_MissionReader.GetConfigurationParam("acceleration", speed);
	m_acceleration = speed/m_radius * 180.0/M_PI;
	m_MissionReader.GetConfigurationParam("initial_angle", m_initial_angle);
	m_MissionReader.GetConfigurationParam("clockwise", m_clockwise);

//	std::cout << m_centerX << " ";
//	std::cout << m_centerY << " ";
//	std::cout << m_radius << " ";
//	std::cout << m_angularSpeed << " ";
//	std::cout << m_clockwise << std::endl;

	initialVel();
	initialPos();

//	std::cout << m_osx << " " << m_osy << std::endl;
}

void AcceleratingCircleSim::doWork(double time) {
	m_lastRunTime = m_thisRunTime;
	m_thisRunTime = time;

	if ( first_iteration ) {
		first_iteration = false;
		return;
	}

//	std::cout << m_currentAngle << std::endl;
//	std::cout << m_osx << " " << m_osy << std::endl;
	if ( m_state == Paused ) return;
//	std::cout << "not paused" << std::endl;

	double timeDiff = m_thisRunTime - m_lastRunTime;

	m_currentVelocity += m_acceleration * timeDiff;
	if ( m_acceleration > 0 && m_currentVelocity > m_maxVelocity ) {
		m_currentVelocity = m_maxVelocity;
	} else if ( m_acceleration < 0 && m_currentVelocity < m_minVelocity ) {
		m_currentVelocity = m_minVelocity;
	}

	double angleDiff = timeDiff * m_currentVelocity;
	if (m_clockwise)
		m_currentAngle += angleDiff;
	else
		m_currentAngle -= angleDiff;
	while ( m_currentAngle > 360 ) m_currentAngle -= 360.0;
	while ( m_currentAngle < -360 ) m_currentAngle += 360.0;

	setPos( m_currentAngle );
}
