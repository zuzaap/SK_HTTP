#include <memory>
#include <iostream>
#include <string>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <sys/select.h>

#include "request_handler.h"
#include "request_parser.h"
#include "connection_manager.h"
#include "response.h"
#include "connection.h"

Connection::Connection(int sock_fd_, const RequestHandler &request_handler, ConnectionManager *connection_manager) :
        sock_fd_(sock_fd_),
        request_parser_(),
        request_handler_(request_handler),
        connection_manager_(connection_manager) {
    std::cout << "Creating connection" << std::endl;
}

void Connection::CreateResponse(RequestParser::ParseResult res) {
    if (res == RequestParser::GOOD) {
        request_handler_.HandleRequest(request_, &response_);
        WriteResponse();
    } else if (res == RequestParser::BAD) {
        response_ = Response::StockResponse(Response::Status::BAD_REQUEST);
        WriteResponse();
    } else if (res == RequestParser::UNKNOWN) {
        assert(false);
    }
}

RequestParser::ParseResult Connection::GetRequest() {
    RequestParser::ParseResult res;
    char buffer[settings.kMaxBufferSize];

    do {
        try {
            size_t bytes_read;
            if (settings.set_timeout && (settings.timeout_sec > 0 || settings.timeout_usec > 0))
                bytes_read = Read(buffer, settings.kMaxBufferSize, settings.timeout_sec, settings.timeout_usec);
            else bytes_read = Read(buffer, settings.kMaxBufferSize);

            if (bytes_read == 0) {
                std::cout << "0 bytes read" << std::endl;
                res = RequestParser::END_CONNECTION;
                break;
            }

            res = request_parser_.Parse(buffer, bytes_read, &request_);
        } catch (...) {
            res = RequestParser::END_CONNECTION;
            break;
        }
    } while (res == RequestParser::UNKNOWN);
    return res;
}

void *Connection::StartRoutine() {
    std::cout << "Starting connection" << std::endl;

    RequestParser::ParseResult res;

    if (!settings.persistent_conn) {
        res = GetRequest();
        CreateResponse(res);
    } else {
        do {
            res = GetRequest();
            CreateResponse(res);
        } while (res == RequestParser::GOOD);
    }
    connection_manager_->Stop(shared_from_this());
    return nullptr;
}

void Connection::Stop() {
    close(sock_fd_);
    std::cout << "Socket closed" << std::endl;
}

void Connection::Run() {
    pthread_create(&thread, nullptr, StaticRoutine, (void *) this);
}

void *Connection::StaticRoutine(void *arg) {
    auto *ithread = (Connection *) arg;
    return ithread->StartRoutine();
}

std::size_t Connection::Read(void *array, std::size_t max_bytes_count) {
    int status = read(sock_fd_, array, max_bytes_count);
    if (status == -1) {
        std::cout << "Read error: " << strerror(errno) << std::endl;
    }
    return status;
}

std::size_t Connection::Read(void *array, std::size_t max_bytes_count, int tv_sec, int tv_usec) {
    fd_set masterfds, readfds;
    FD_ZERO(&masterfds);
    FD_SET(sock_fd_, &masterfds);
    memcpy(&readfds, &masterfds, sizeof(fd_set));
    timeval timeout;
    timeout.tv_sec = tv_sec;
    timeout.tv_usec = tv_usec;
    if (select(sock_fd_ + 1, &readfds, nullptr, nullptr, &timeout) < 0) {
        std::cout << "Select error: " << strerror(errno) << std::endl;
    }

    int status = 0;
    if (FD_ISSET(sock_fd_, &readfds)) {
        status = read(sock_fd_, array, max_bytes_count);
        if (status == -1) {
            std::cout << "Read error: " << strerror(errno) << std::endl;
        }
    } else {
        std::cout << "SOCK FD: " << sock_fd_ << std::endl;
    }
    return status;
}

void Connection::WriteResponse() {
    std::string response_string = response_.ToString();
    std::cout << response_string.size() <<std::endl;
    if (write(sock_fd_, response_string.c_str(), response_string.size()) == -1)
        std::cout << "Write error: " << strerror(errno) << std::endl;
}