#include "subtitle.h"
using namespace std;

Subtitle::Subtitle(int seq, int start, int end, std::string text) {
	m_seq = seq;
	m_start = start;
	m_end = end;
	m_text = text;
}

int Subtitle::get_seq() {
	return m_seq;
}

int Subtitle::get_start() {
	return m_start;
}

int Subtitle::get_end() {
	return m_end;
}

string Subtitle::get_text() {
	return m_text;
}

string Subtitle::get_formatted() {
	string info = "Subtitle # " + to_string(m_seq) + " (" + to_string(m_start);
	info += " -> " + to_string(m_end) + "): " + m_text;
	return info;
}
