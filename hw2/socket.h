#ifndef __SOCKET_H__

#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

class Socket {
private:
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname;
  const char *port;

public:
  Socket(const char *hostname, const char *port)
      : hostname(hostname), port(port) {}
  Socket(const Socket &rhs) : hostname(rhs.hostname), port(rhs.port) {}
  Socket &operator=(const Socket &rhs) {
    if (this != &rhs) {
      Socket temp(rhs);
      std::swap(status, temp.status);
      std::swap(socket_fd, temp.socket_fd);
      std::swap(host_info, temp.host_info);
      std::swap(host_info_list, temp.host_info_list);
      std::swap(hostname, temp.hostname);
      std::swap(port, temp.port);
    }
    return *this;
  }
  int init_server();
  int init_client();
  ssize_t send_all(int sockfd, const void *buf, size_t len);
  ~Socket() {}
  // constructor and copy constructor
};

#endif
