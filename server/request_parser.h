#ifndef SRC_SERVER_REQUEST_PARSER_H_
#define SRC_SERVER_REQUEST_PARSER_H_

#include "request.h"

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <regex>

class RequestParser {
public:
    enum ParseResult { GOOD, BAD, UNKNOWN, END_CONNECTION };

    ParseResult Parse(const char *buffer, size_t bytes_read, Request *request);

    RequestParser();

private:
    enum ParserState { REQUEST_LINE, HEADERS, BODY, ERROR, SUCCESS };

    ParserState state_;

    const std::regex request_line_regex_;

    std::string GetLine(const char *buffer);

    std::pair<std::string, std::string> *ParseHeader(const std::string &line);

    void PopulateRequest(Request *request);

    void Reset();

    std::string temp_URL_;
    std::string temp_HTTP_version_;
    Request::Method temp_HTTP_method_;
    std::string temp_body_;
    std::map<std::string, std::string> temp_headers_;
    int content_length_{};
    int position_in_content_{};


    std::string delimiter_ = "\r\n";
    std::string header_delimiter_ = ": ";
};

#endif  // SRC_SERVER_REQUEST_PARSER_H_
