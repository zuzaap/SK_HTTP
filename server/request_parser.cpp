#include <string>
#include <map>
#include <utility>
#include <regex>
#include <iostream>
#include <algorithm>
#include "request_parser.h"
#include "request.h"


RequestParser::RequestParser() : state_(REQUEST_LINE), request_line_regex_(
        R"(^(OPTIONS|GET|HEAD|POST|PUT|DELETE|TRACE|CONNECT) (.*) HTTP/(\d\.\d)$)") {}

RequestParser::ParseResult RequestParser::Parse(const char *buffer, size_t bytes_read, Request *request) {
    Reset();
    size_t cursor = 0;
    std::string line;
    std::cout<<buffer<<std::endl;
    while (cursor < bytes_read) {
        switch (state_) {
            case REQUEST_LINE: {
                line = GetLine(buffer + cursor);
                cursor = cursor + line.length() + delimiter_.length();

                std::cmatch match;
                if (std::regex_match(line.c_str(), match, request_line_regex_)) {
                    temp_HTTP_method_ = Request::methodString(match[1]);
                    temp_URL_ = match[2];
                    temp_HTTP_version_ = match[3];
                    state_ = HEADERS;
                } else state_ = ERROR;
                break;
            }
            case HEADERS:
                line = GetLine(buffer + cursor);
                cursor = cursor + line.length() + delimiter_.length();
                if (line.length() > 0) {
                    std::pair<std::string, std::string> *header = ParseHeader(line);
                    if (header != nullptr) {
                        temp_headers_.insert(*header);
                        if (header->first == "Content-Length")
                            content_length_ = std::stoi(header->second);
                    } else state_ = ERROR;
                } else {
                    if (content_length_ == -1) state_ = SUCCESS;
                    else if (temp_HTTP_method_ == Request::PUT) state_ = BODY;
                    else state_ = ERROR;
                }
                break;
            case BODY:

                if (content_length_ == -1) {
                    state_ = ERROR;
                    break;
                }
                if (content_length_ == 0) {
                    state_ = SUCCESS;
                    break;
                }
                temp_body_ += buffer[cursor++];
                position_in_content_++;
                if (position_in_content_ >= content_length_) {
                    state_ = SUCCESS;
                    break;
                }
                break;
            case ERROR:
                break;
            case SUCCESS:
                break;
            default:
                state_ = ERROR;
                break;
        }
    }
    if (state_ == ERROR) {
        return RequestParser::BAD;
    } else if (state_ == SUCCESS || (state_ == BODY && temp_HTTP_method_ == Request::Method::HEAD)) {
        std::cout << "HTTP Method: " << temp_HTTP_method_ << std::endl;
        std::cout << "URL: " << temp_URL_ << std::endl;
        std::map<std::string, std::string>::iterator it;
        for (it = temp_headers_.begin(); it != temp_headers_.end(); ++it)
            std::cout << "HEADER: " << it->first << ": " << it->second << std::endl;
        if (temp_HTTP_method_ == Request::PUT) std::cout << "Body: " << temp_body_ << std::endl;
        PopulateRequest(request);
        return RequestParser::GOOD;
    } else return RequestParser::UNKNOWN;
}

void RequestParser::Reset() {
    if (state_ != BODY) {
        temp_body_ = "";
        temp_URL_ = "";
        temp_HTTP_version_ = "";
        temp_HTTP_method_ = Request::Method::GET;
        state_ = REQUEST_LINE;
        temp_headers_.clear();
        content_length_ = -1;
        position_in_content_ = 0;
    }
}

void RequestParser::PopulateRequest(Request *request) {
    request->content() = temp_body_;
    request->method() = temp_HTTP_method_;
    request->getUrl() = temp_URL_;
    request->headers().clear();
    request->headers() = temp_headers_;
}

std::pair<std::string, std::string> *RequestParser::ParseHeader(const std::string &line) {
    size_t occurence = line.find(header_delimiter_);
    if (std::string::npos != occurence && occurence > 0) {
        std::string key = line.substr(0, occurence);
        std::string value = line.substr(occurence + header_delimiter_.length());
        return new std::pair<std::string, std::string>(key, value);
    } else return nullptr;
}

std::string RequestParser::GetLine(const char *buffer) {
    std::string bufferString(buffer);
    size_t occurence = bufferString.find(delimiter_);
    if (std::string::npos == occurence) return bufferString;
    else return bufferString.substr(0, occurence);
}
