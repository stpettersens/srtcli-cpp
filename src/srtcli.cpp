/*
    srtcli (c++)
    Utility to playback subtitles on the command line.

    Copyright 2016 Sam Saint-Pettersen.
    Released under the MIT/X11 License.

    Port of original Rust program to C++.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <cstdlib>
#include <thread>
#include <boost/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include "clock.h"
#include "subtitle.h"
using namespace std;

void display_version() {
    cout << "srtcli (c++) v. 1.0 (using Boost " << BOOST_LIB_VERSION << ")." << endl;
    exit(0);
}

void display_usage(string program, int exit_code) {
    cout << "\nsrtcli" << endl;
    cout << "Utility to playback subtitles on the command line." << endl;
    cout << "\nCopyright 2016 Sam Saint-Pettersen." << endl;
    cout << "Released under the MIT/X11 License." << endl;
    cout << "\nUsage: " << program << " -f|--file <file.srt> -c|--clock ";
    cout << "[-h|--help][-v|--version]" << endl;
    cout << "\n-f|--file: Subtitle file (SubRip Text) to playback." << endl;
    cout << "-c|--clock: Display a playback clock." << endl;
    cout << "-h|--help: Display this help information and exit" << endl;
    cout << "-v|--version: Display version information and exit." << endl;
    exit(exit_code);
}

void display_error(string program, string err) {
    cout << "Error: " << err << "." << endl;
    display_usage(program, -1);
}

void clear_text(int lines) {
    for(int i = 0; i < lines; i++) {
        cout << "" << endl;
    }
}

int convert_time_to_ms(string timestamp) {
    int time = 0;
    smatch m;
    regex p("([0-9]{2}):([0-9]{2}):([0-9]{2}),([0-9]{3})");
    regex_match(timestamp, m, p);
    int hours = atoi(m[1].str().c_str());
    int minutes = atoi(m[2].str().c_str());
    int seconds = atoi(m[3].str().c_str());
    int ms = atoi(m[4].str().c_str());

    time = (hours * 3600) + (minutes * 60) + seconds;
    time = (time * 1000) + ms;

    return time;
}

bool is_valid(string data) {
    bool valid = false;
    if(regex_match(data, regex("^[0-9]+$"))) {
        valid = true;
    }
    return valid;
}

vector<Subtitle> parse_subtitles(string program, string input) {
    vector<string> lines;
    string l;
    ifstream f(input.c_str());
    while(getline(f, l)) {
        lines.push_back(l);
    }
    f.close();

    if(!is_valid(lines[0])) {
        display_error(program, "Input file is not valid subtitles");
    }

    vector<int> seqs;
    vector<int> starts;
    vector<int> ends;
    vector<string> contents;
    for(int i = 0; i < lines.size(); i++) {
        bool is_timecode = false;
        if(regex_match(lines[i], regex("^[0-9]+$"))) {
            seqs.push_back(atoi(lines[i].c_str()));
            continue;
        }
        smatch m;
        string ts = "([0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3})";
        if(regex_match(lines[i], m, regex(ts + " --> " + ts))) {
            starts.push_back(convert_time_to_ms(m[1].str()));
            ends.push_back(convert_time_to_ms(m[2].str()));
            is_timecode = true;
            continue;
        }
        if(lines[i].empty()) {
            contents.push_back("|");
        }
        else if(!is_timecode) {
            contents.push_back(lines[i]);
            continue;
        }
    }

    vector<string> text;
    string joined = boost::join(contents, "\n");
    boost::split(text, joined, boost::is_any_of("|"));

    vector<Subtitle> subtitles;
    for(int i = 0; i < seqs.size(); i++) {
        subtitles.push_back(Subtitle(seqs[i], starts[i], ends[i], text[i]));
    }

    return subtitles;
}

void playback_subtitles(string program, string input, bool use_clock) {
    vector<Subtitle> subtitles = parse_subtitles(program, input);
    int runtime = subtitles[subtitles.size() - 1].get_end();
    Clock clock;
    clock.set_time_ms(runtime);
    clear_text(50);

    stringstream info;
    info << boost::format{"Playing back: '%s' (Runtime: %s [%dms])..."} % input % clock.get_time() % runtime;
    cout << info.str() << endl;
    boost::this_thread::sleep_for(boost::chrono::seconds(3));
    clock.set_time_ms(0);
    cout << clock.get_time() << endl;
    clear_text(50);

    int i = 0;
    auto start = boost::chrono::high_resolution_clock::now();
    auto now = start;
    bool in_subtitle = false;
    while(true) {

        auto time = boost::chrono::duration_cast<boost::chrono::milliseconds>(now - start).count();

        clock.set_time_ms(time);
        if(time % 1000 == 0 && use_clock && !in_subtitle) {
            clear_text(40);
            cout << clock.get_time() << endl;
        }

        if(time == subtitles[i].get_start()) {
            clear_text(50);
            in_subtitle = true;
            cout << subtitles[i].get_text() << endl;
        }

        if(time == subtitles[i].get_end()) {
            i++;
            in_subtitle = false;
            clear_text(50);
        }

        if(i == subtitles.size()) {
            break;
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
        now = boost::chrono::high_resolution_clock::now();
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    string program = string(argv[0]);
    string input = "";
    bool use_clock = false;

    if(argc > 1) {
        for(int i = 1; i < argc; i++) {
            if(regex_match(argv[i], regex("-h|--help"))) {
                display_usage(program, 0);
            }
            if(regex_match(argv[i], regex("-v|--version"))) {
                display_version();
            }
            if(regex_match(argv[i], regex("-f|--file"))) {
                if(i < argc - 1) input = argv[i+1];
            }
            if(regex_match(argv[i], regex("-c|--clock"))) {
                use_clock = true;
            }
        }

        if(input.empty()) {
            display_error(program, "No subtitle file specified");
        }

        playback_subtitles(program, input, use_clock);
    }
    else {
        display_error(program, "No options specified");
    }

    return 0;
}
