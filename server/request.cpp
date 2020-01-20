#include "request.h"
#include <string>
#include<cassert>

Request::Method Request::methodString(const std::string &input) {
    if (input == "GET")
        return Method::GET;
    else if (input == "HEAD")
        return Method::HEAD;
    else if (input == "PUT")
        return Method::PUT;
    else if (input == "DELETE")
        return Method::DELETE;
    else
        assert(false);
}
