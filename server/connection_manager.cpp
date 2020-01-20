#include "connection_manager.h"
#include "connection.h"
#include <pthread.h>

ConnectionManager::ConnectionManager() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&mutex_, &attr);
}

ConnectionManager::~ConnectionManager() {
    pthread_mutex_destroy(&mutex_);
}

void ConnectionManager::Start(const std::shared_ptr<Connection> &connection) {
    Lock();
    connections_.insert(connection);
    Unlock();
    connection->Run();
}

void ConnectionManager::Stop(const std::shared_ptr<Connection> &connection) {
    connection->Stop();
    Lock();
    connections_.erase(connection);
    Unlock();
}

void ConnectionManager::StopAll() {
    for (const auto &c : connections_)
        c->Stop();
    Lock();
    connections_.clear();
    Unlock();
}

void ConnectionManager::Lock() {
    pthread_mutex_lock(&mutex_);
}

void ConnectionManager::Unlock() {
    pthread_mutex_unlock(&mutex_);
}

