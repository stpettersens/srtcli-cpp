#ifndef SUBTITLE_H
#define SUBTITLE_H

#include <string>

class Subtitle {
private:
	int m_seq;
	int m_start;
	int m_end;
	std::string m_text;
public:
	Subtitle(int seq, int start, int end, std::string text);
	int get_seq();
	int get_start();
	int get_end();
	std::string get_text();
	std::string get_formatted();
};

#endif
