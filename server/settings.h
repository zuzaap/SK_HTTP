#ifndef SK_HTTP_SETTINGS_H
#define SK_HTTP_SETTINGS_H

#include <cstdlib>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

class Settings {
public:
    std::string address;
    std::string port;
    std::string root_dir;
    bool persistent_conn;
    int timeout_sec;
    int timeout_usec;
    bool set_timeout;
    int listenerBacklog;
    size_t kMaxBufferSize;

    Settings() {
//        address = "192.168.1.113";
        address = "192.168.43.71";
        port = "8080";
        root_dir = "./files";
        persistent_conn = true;
        set_timeout = true;
        timeout_sec = 5;
        timeout_usec = 0;
        listenerBacklog = 5;
        kMaxBufferSize = 8192;
    }

    void init() {
        if (access(root_dir.c_str(), F_OK)) {
            if (!mkdir(root_dir.c_str(), 0777))
                std::cout << "File directory created" << std::endl;
        }
    }

};

#endif
