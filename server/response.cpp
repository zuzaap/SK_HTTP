#include "response.h"

#include <string>
#include <sstream>
#include <iostream>


Response::Response() {
    statuses_[200] = "OK";
    statuses_[201] = "Created";
    statuses_[204] = "No Content";
    statuses_[400] = "Bad Request";
    statuses_[404] = "Not Found";
    statuses_[500] = "Internal Server Error";
    statuses_[501] = "Not Implemented";
}

void Response::Clear() {
    headers_.clear();
    content_.clear();
    status_ = Response::NO_CONTENT;
}

Response Response::StockResponse(Status status) {
    Response response;
    response.status() = status;
    response.content() = response.ContentString();
    response.headers().clear();
    response.headers()["Content-Length"] = std::to_string(response.content().size());
    response.headers()["Content-Type"] = "text/html";
    return response;
}

std::string Response::ContentString() const {
    int status = status_;
    std::stringstream ss;
    ss << "<html><head><title>" << statuses()[status] << "</title></head><body><h1>" << std::to_string(status) << " "
       << statuses()[status] << "</h1></body></html>";
    return ss.str();
}


std::string Response::ToString() const {
    std::stringstream result;
    result << "HTTP/1.1" << " " << std::to_string(status_) << " " << statuses()[status_] << "\r\n";

    for (const auto &h : headers_)
        result << h.first << ": " << h.second << "\r\n";

    result << "\r\n" << content_;
    return result.str();
}

