#ifndef SRC_HTTP_REQUEST_H_
#define SRC_HTTP_REQUEST_H_

#include <string>
#include <vector>
#include <map>

class Request {
public:
    enum Method {
        GET,
        HEAD,
        PUT,
        DELETE
    };

    Method &method() { return method_; }

    std::string &getUrl() { return url; }

    static Request::Method methodString(const std::string &input);

    std::map<std::string, std::string> &headers() { return headers_; }

    std::string &content() { return content_; }

private:
    Method method_;
    std::string url;
    std::map<std::string, std::string> headers_;
    std::string content_;


};


#endif  // SRC_HTTP_REQUEST_H_
