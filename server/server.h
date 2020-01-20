#include "connection_manager.h"
#include "request_handler.h"

class Server {
public:
    Server();

    ~Server();

    void Run();

    int Accept();

private:

    ConnectionManager connection_manager_;
    RequestHandler request_handler_;
    Settings settings;
    int sock_fd_;
};