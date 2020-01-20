#include <iostream>
#include <fstream>
#include <regex>
#include <sys/stat.h>
#include <unistd.h>

#include "request_handler.h"
#include "request.h"
#include "response.h"

RequestHandler::RequestHandler() {
    mime_types_["js"] = "application/javascript";
    mime_types_["gif"] = "image/gif";
    mime_types_["jpeg"] = "image/jpeg";
    mime_types_["jpg"] = "image/jpeg";
    mime_types_["png"] = "image/png";
    mime_types_["css"] = "text/css";
    mime_types_["html"] = "text/html";
}


bool RequestHandler::findPath(const std::string &url, std::string *path) const {

    std::regex rgx("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");
    std::smatch match;
    if (std::regex_search(url.begin(), url.end(), match, rgx)) {
        *path = match[5];
    }
    return true;
}

std::string RequestHandler::GetMIMEType(const std::string &full_path) const {
    std::string extension;
    std::size_t last_dot_pos = full_path.find_last_of('.');
    if (last_dot_pos != std::string::npos) {
        extension = full_path.substr(last_dot_pos + 1);
    }
    try {
        return mime_types_.at(extension);
    } catch (std::out_of_range &) {
        return "text/plain";
    }
}

void RequestHandler::HandleRequest(Request request, Response *response) const {
    response->Clear();
    std::string path;

    if (!findPath(request.getUrl(), &path)) {
        *response = Response::StockResponse(Response::BAD_REQUEST);
        return;
    }
    std::string full_path = settings.root_dir + path;

    switch (request.method()) {
        case Request::GET:
            CreateResponse(full_path, response);
            break;
        case Request::HEAD:
            CreateResponse(full_path, response);
            response->content() = "";
            break;
        case Request::PUT:
            WriteFile(full_path, request.content(), response);
            break;
        case Request::DELETE:
            DeleteFile(full_path, response);
            break;
        default:
            *response = Response::StockResponse(Response::NOT_IMPLEMENTED);
    }
}

void RequestHandler::CreateResponse(const std::string &full_path, Response *response) const {
    std::ifstream is(full_path, std::ios::in | std::ios::binary);
    if (!is.is_open()) {
        *response = Response::StockResponse(Response::NOT_FOUND);
        return;
    }
    response->status() = Response::OK;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0) {
        response->content().append(buf, is.gcount());
    }

    response->headers().clear();
    response->headers()["Content-Length"] = std::to_string(response->content().size());
    response->headers()["Content-Type"] = GetMIMEType(full_path);
}

void RequestHandler::DeleteFile(const std::string &full_path, Response *response) const {
    try {
        if (std::remove(full_path.c_str())) *response = Response::StockResponse(Response::NOT_FOUND);
        else *response = Response::StockResponse(Response::NO_CONTENT);
    } catch (...) {
        *response = Response::StockResponse(Response::INTERNAL_SERVER_ERROR);
    }
}

void RequestHandler::WriteFile(const std::string &full_path, const std::string &content, Response *response) const {
    bool new_file = false;

    if (access(full_path.c_str(), F_OK)) {
        new_file = true;

        std::regex rgx(".*[^/]+(?=/[^/]+$)");
        std::smatch match;
        std::string parent_path;

        if (std::regex_search(full_path.begin(), full_path.end(), match, rgx)) {
            parent_path = match[0];
            if (!mkdir(parent_path.c_str(), 0777))
                std::cout << parent_path << " directory created" << std::endl;
        }
    }

    std::ofstream os(full_path, std::ios::out | std::ios::binary);
    if (!os) {
        *response = Response::StockResponse(Response::INTERNAL_SERVER_ERROR);
        return;
    }
    os << content;
    os.close();

    if (new_file) *response = Response::StockResponse(Response::CREATED);
    else *response = Response::StockResponse(Response::OK);
}