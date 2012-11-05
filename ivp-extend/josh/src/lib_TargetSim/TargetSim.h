/*
 * TimedAcomms.h
 *
 *  Created on: Oct 15, 2012
 *      Author: josh
 */

#include <string>
#include <MOOSLib.h>

#ifndef TARGETSIM_H_
#define TARGETSIM_H_

class BaseSim {
public:
	std::pair<double, double> getTargetPos();
	void getTargetPos(double & x, double & y);

	virtual void start() { reset(); resume(); }
	virtual void pause() = 0;
	virtual void resume() = 0;
	virtual void reset() = 0;

	virtual void doWork(double time) = 0;

	virtual void setConfiguration(CProcessConfigReader & m_MissionReader) = 0;

protected:
	double m_osx, m_osy;
	double m_thisRunTime, m_lastRunTime;

	enum STATE { Paused, Running };
	STATE m_state;

};

class CircleSim: public BaseSim {
public:
	CircleSim() {
		m_state = Paused;
		first_iteration = true;
	}

	void pause();
	void resume();
	void reset();

	void doWork(double time);

	void setConfiguration(CProcessConfigReader & m_MissionReader);

private:
	double m_centerX, m_centerY, m_radius, m_angularSpeed, m_initial_angle;
	bool m_clockwise;

	double m_currentAngle;

	bool first_iteration;

	void initialPos();
	void setPos( double angle );
};

#endif /* TARGETSIM_H_ */
