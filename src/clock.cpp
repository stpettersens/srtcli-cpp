#include "clock.h"
using namespace std;

Clock::Clock() {
	m_hours = 0;
	m_minutes = 0;
	m_seconds = 0;
}

void Clock::set_time_ms(int ms) {
	m_seconds = (ms / 1000) % 60;
	m_minutes = (ms / (1000 * 60)) % 60;
	m_hours = (ms / (1000 * 60 * 60)) % 24;
}

void Clock::set_time_secs(int seconds) {
	this->set_time_ms(seconds * 1000);
}

string Clock::get_time() {
	stringstream time;
	time << boost::format{"%02d:%02d:%02d"} % m_hours % m_minutes % m_seconds;
	return time.str();
}
