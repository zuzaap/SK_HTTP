#include <string>
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <netinet/in.h>

#include "connection_manager.h"
#include "server.h"

Server::~Server() {
    close(sock_fd_);
    std::cout << "Socket closed" << std::endl;
    connection_manager_.StopAll();
}

Server::Server(): connection_manager_(), request_handler_() {
  std::cout <<  "Address: " << settings.address << std::endl;
  std::cout <<  "Port: " << settings.port << std::endl;

  sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd_ == -1) {
      std::cout << "Socket open error: " << strerror(errno) << std::endl;
  }
  char option = 1;
  if (setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &option,  sizeof(int)) == -1) {
      std::cout << "Set reuse address error: " << strerror(errno) << std::endl;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(settings.port.c_str()));
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock_fd_, (sockaddr*)&addr, sizeof(addr)) == -1) {
      std::cout << "Bind error: " << strerror(errno) << std::endl;
  }
  if (listen(sock_fd_, settings.listenerBacklog) == -1) {
      std::cout << "Listen error: " << strerror(errno) << std::endl;
  }
}

void Server::Run() {
  std::cout << "Server is running" << std::endl;

  while (true) {
    try {
      int socket = Server::Accept();
      connection_manager_.Start(std::make_shared<Connection>(socket, request_handler_, &connection_manager_));
    }
    catch (...) {
        std::cout << "Unhandled exception" << std::endl;
        break;
    }
  }
}

int Server::Accept() {
    std::cout <<  "Waiting for socket" << std::endl;

    socklen_t socklen;
    sockaddr_in in_addr;
    socklen = sizeof(in_addr);

    int new_sock_fd = accept(sock_fd_, (sockaddr*)&in_addr, &socklen);
    if (new_sock_fd == -1) {
        std::cout << "Accept error: " << strerror(errno) << std::endl;
    }

    std::cout << "Accepted connection, socket id: " << new_sock_fd << std::endl;

    return new_sock_fd;
}
