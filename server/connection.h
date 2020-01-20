#ifndef SRC_SERVER_CONNECTION_H_
#define SRC_SERVER_CONNECTION_H_

#include <memory>

#include "request_parser.h"
#include "request.h"
#include "response.h"
#include "settings.h"


class RequestHandler;

class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(int sock_fd_, const RequestHandler &request_handler, ConnectionManager *connection_manager);

    void Stop();

    void Run();

    size_t Read(void *array, std::size_t max_bytes_count);

    size_t Read(void *array, std::size_t max_bytes_count, int tv_sec, int tv_usec);

private:
    void *StartRoutine();

    RequestParser::ParseResult GetRequest();

    void CreateResponse(RequestParser::ParseResult res);

    void WriteResponse();

    int sock_fd_;
    RequestParser request_parser_;
    const RequestHandler &request_handler_;
    ConnectionManager *connection_manager_;

    Request request_;
    Response response_;

    Settings settings;
    pthread_t thread{};

    static void *StaticRoutine(void *arg);

};


#endif  // SRC_SERVER_CONNECTION_H_
