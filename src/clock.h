#ifndef CLOCK_H
#define CLOCK_H

#include <boost/format.hpp>
#include <sstream>
#include <string>

class Clock {
private:
	int m_hours;
	int m_minutes;
	int m_seconds;
public:
	Clock();
	void set_time_ms(int ms);
	void set_time_secs(int seconds);
	std::string get_time();
};

#endif
