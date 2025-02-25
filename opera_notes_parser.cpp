#include "opera_notes_parser.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/regex.hpp>

Note::Note(std::string note, std::string date, std::string url, std::string title) {
    note_ = note;
    date_ = date;
    url_ = url;
    title_ = title;
}

Note::~Note() {}

bool Note::operator <(const Note & other) const {
    return date_ < other.date();
}

OperaNotesParser::OperaNotesParser() {}

OperaNotesParser::OperaNotesParser(std::string file_name)
{
    std::size_t length;
    char * bytes;
    std::string file_string;
    std::ifstream is;

    is.open(file_name.c_str(), std::ios::binary);

    is.seekg(0, std::ios::end);
    length = is.tellg();
    is.seekg(0, std::ios::beg);

    bytes = new char[length];

    is.read(bytes, length);
    is.close();

    file_string = std::string(bytes, length);

    std::size_t pos = 0, prev = 0, note_index = 0;

    while (pos != std::string::npos) {
        std::ostringstream note_stringstream;

        prev = pos;
        pos = file_string.find("\n\n#", pos + 3);

        std::string note = file_string.substr(prev + 3, pos - prev - 3);
        std::string url = "";
        std::string date = "";
        std::string title = "";

        if (note.length() < 4 || note.substr(0, 4) != "NOTE")
            continue;

        std::size_t text_start = note.find("\n\tNAME=");
        if (text_start == std::string::npos)
            continue;

        text_start += 7;

        std::size_t text_end = note.find("\n\t", text_start);
        if (text_end == std::string::npos)
            continue;

        std::size_t url_start = note.find("\n\tURL=");
        if (url_start != std::string::npos) {
            url_start += 6;
            std::size_t url_end = note.find("\n\t", url_start);
            if (url_end != std::string::npos) {
                url = note.substr(url_start, url_end - url_start);
            }
        }

        std::size_t created_start = note.find("\n\tCREATED=");
        if (created_start != std::string::npos) {
            created_start += 10;
            std::size_t created_end = note.find("\n\t", created_start);
            if (created_end != std::string::npos) {
                date = note.substr(created_start, created_end - created_start);
            } else {
                date = note.substr(created_start);
            }
        }

        note = note.substr(text_start, text_end - text_start);
        note = boost::regex_replace(note, boost::regex("\x02\x02"), "\n");
        note = trim(note);

        title = boost::regex_replace(note, boost::regex("^([^\n]{1,100}).*"), "\\1");

        if (title.length() > 97)
            title = title.substr(0, 97) + "...";

        notes_.push_back(Note(note, date, url, title));

        note_index++;
    }

    std::sort(notes_.begin(), notes_.end());
    std::reverse(notes_.begin(), notes_.end());

    delete[] bytes;
}

OperaNotesParser::~OperaNotesParser() {}

long OperaNotesParser::stringToLong(const std::string &str) {
    long l;
    std::istringstream in(str);
    in >> l;
    return l;
}

std::string OperaNotesParser::formatUnixTime(const std::string &str) {
    long c = stringToLong(str);
    time_t t = c;
    struct tm *timeinfo = localtime(&t);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %I:%M:%S%p", timeinfo);
    return std::string(buffer);
}

std::string OperaNotesParser::trim(const std::string &str) {
    size_t s = str.find_first_not_of(" \f\n\r\t\v");
    size_t e = str.find_last_not_of (" \f\n\r\t\v");

    std::string newstr;

    if (( std::string::npos == s) || ( std::string::npos == e))
        newstr = "";
    else
        newstr = str.substr(s, e-s+1);

    return newstr;
}
