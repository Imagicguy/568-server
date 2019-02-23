#ifndef __PROXY_H__
#include "response.h"
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

#include <vector>
class Proxy {
private:
  int client_fd;
  int server_fd;
  std::string ip;
  const char *port_num;

public:
  Proxy(int client, int server, std::string ipAdd, const char *num)
      : client_fd(client), server_fd(server), ip(ipAdd), port_num(num) {}
  void error_check(int &ID, int error) {
    switch (error) {
    case 1: {
      std::cout << ID << " : "
                << "ERROR recv() failed!" << std::endl;
      break;
    }

    case 2: {
      std::cout << ID << " : "
                << "ERROR send() failed!" << std::endl;
      break;
    }

    case 3: {
      std::cout << ID << " : "
                << "ERROR select() failed!" << std::endl;
      break;
    }

    case 4: {
      std::cout << ID << " : "
                << "ERROR connect() failed" << std::endl;
      break;
    }

    case 400: {
      std::cout << ID << " : "
                << "ERROR BAD REQUEST" << std::endl;
      break;
    }
    case 404: {
      std::cout << ID << " : "
                << "ERROR NOT FOUND" << std::endl;
      break;
    }
    case 411: {
      std::cout << ID << " : "
                << "ERROR LENGTH REQUIRED" << std::endl;
      break;
    }
    case 502: {
      std::cout << ID << " : "
                << "ERROR BAD GATEWAY" << std::endl;
      break;
    }
    default: { return; }
    }
  };
  void request_parse(){}; // need parse class
  void get_handle(){};    // need cache
  void post_handle(){};
  void connect_handle(){};
  void process(std::unordered_map<std::string, response> &cache, int ID) {
    std::string content;
    int recv_bytes = 1;
    while (recv_bytes > 0) {
      char buffer[UINT_MAX];
      memset(buffer, 0, UINT_MAX);
      recv_bytes = recv(client_fd, buffer, UINT_MAX, 0);
      std::cout << "received is " << recv_bytes << std::endl;
      if (recv_bytes < 0) {
        error_check(ID, 1);
        return;
      }
      content += std::string(buffer);
    }
  }
};
#endif
