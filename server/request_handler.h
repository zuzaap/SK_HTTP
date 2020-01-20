#ifndef SRC_SERVER_REQUEST_HANDLER_H_
#define SRC_SERVER_REQUEST_HANDLER_H_


#include "settings.h"
#include "response.h"
#include "request.h"
#include <map>


class RequestHandler {
public:
    RequestHandler();

    void HandleRequest(Request request, Response *response) const;

private:
    bool findPath(const std::string &url, std::string *path) const;

    RequestHandler(const RequestHandler &);

    void CreateResponse(const std::string &full_path, Response *response) const;

    void DeleteFile(const std::string &full_path, Response *response) const;

    void WriteFile(const std::string &full_path, const std::string &content, Response *response) const;

    std::string GetMIMEType(const std::string &extension) const;

    Settings settings;

    std::map<std::string, std::string> mime_types_;
};

#endif  // SRC_SERVER_REQUEST_HANDLER_H_
