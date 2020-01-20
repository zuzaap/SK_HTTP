#ifndef SRC_HTTP_RESPONSE_H_
#define SRC_HTTP_RESPONSE_H_

#include <vector>
#include <string>
#include <map>


class Response {
public:
    enum Status {
        OK = 200,
        CREATED = 201,
        NO_CONTENT = 204,
        BAD_REQUEST = 400,
        NOT_FOUND = 404,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
    };

    Response();

    void Clear();

    Status &status() { return status_; }

    static Response StockResponse(Status status);

    std::map<std::string, std::string> &headers() { return headers_; }

    std::map<int, std::string> statuses() const { return statuses_; }

    std::string &content() { return content_; }

    std::string ToString() const;


private:
    std::string ContentString() const;

    std::map<std::string, std::string> headers_;
    std::map<int, std::string> statuses_;
    std::string content_;

    Status status_;

};


#endif  // SRC_HTTP_RESPONSE_H_
