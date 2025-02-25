#ifndef OPERANOTESPARSER_H
#define OPERANOTESPARSER_H

#include <string>
#include <vector>

class Note {
private:
    std::string note_;
    std::string date_;
    std::string url_;
    std::string title_;

public:
    Note(std::string note, std::string date, std::string url, std::string title);
    virtual ~Note();
    std::string const & note() const { return note_; }
    std::string const & date() const { return date_; }
    std::string const & url() const { return url_; }
    std::string const & title() const { return title_; }
    virtual bool operator <(const Note & other) const;
};

class OperaNotesParser
{
private:
    std::string file_name_;
    std::vector<Note> notes_;
public:
    OperaNotesParser();
    OperaNotesParser(std::string file_name);
    ~OperaNotesParser();
    static long stringToLong(const std::string &str);
    static std::string formatUnixTime(const std::string &str);
    std::vector<Note> const & notes() const { return notes_; }
    static std::string trim(const std::string &str);
};

#endif // OPERANOTESPARSER_H
