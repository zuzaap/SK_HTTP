#ifndef SRC_SERVER_CONNECTION_MANAGER_H_
#define SRC_SERVER_CONNECTION_MANAGER_H_

#include <set>
#include <memory>
#include <pthread.h>

#include "connection.h"

class ConnectionManager {
public:
    ConnectionManager();

    ~ConnectionManager();

    void Start(const std::shared_ptr<Connection> &connection);

    void Stop(const std::shared_ptr<Connection> &connection);

    void StopAll();

    void Lock();

    void Unlock();

private:
    std::set<std::shared_ptr<Connection>> connections_;
    pthread_mutex_t mutex_{};

};

#endif  // SRC_SERVER_CONNECTION_MANAGER_H_
