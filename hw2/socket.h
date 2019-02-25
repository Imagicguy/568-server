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
  /*int init_server();
  int init_client();
  ssize_t send_all(int sockfd, const void *buf, size_t len);
  */

  int init_server() {
    struct addrinfo **p_host_info_list = &host_info_list;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, p_host_info_list);
    if (status != 0) {
      std::cout << "error: getaddrinfo" << std::endl;
      exit(EXIT_FAILURE);
    }

    socket_fd =
        socket((*p_host_info_list)->ai_family, (*p_host_info_list)->ai_socktype,
               (*p_host_info_list)->ai_protocol);
    if (socket_fd == -1) {
      std::cout << "error: server socket" << std::endl;
      exit(EXIT_FAILURE);
    } // if

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    status = bind(socket_fd, (*p_host_info_list)->ai_addr,
                  (*p_host_info_list)->ai_addrlen);
    if (status == -1) {
      std::cout << "error: bind" << std::endl;
      exit(EXIT_FAILURE);
    }
    status = listen(socket_fd, 100);
    if (status == -1) {
      std::cout << "error: listen" << std::endl;
      exit(EXIT_FAILURE);
    }
    return socket_fd;
  }

  int init_client() {
    struct addrinfo **p_host_info_list = &host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &host_info, p_host_info_list);
    if (status != 0) {
      std::cout << "error: getaddrinfo" << std::endl;
      exit(EXIT_FAILURE);
    }

    socket_fd =
        socket((*p_host_info_list)->ai_family, (*p_host_info_list)->ai_socktype,
               (*p_host_info_list)->ai_protocol);
    if (socket_fd == -1) {
      std::cout << "error: client socket" << std::endl;
      exit(EXIT_FAILURE);
    }

    status = connect(socket_fd, (*p_host_info_list)->ai_addr,
                     (*p_host_info_list)->ai_addrlen);
    if (status == -1) {
      std::cout << "error: connect" << std::endl;
      exit(EXIT_FAILURE);
    }
    return socket_fd;
  }

  ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total = 0;
    while (1) {
      ssize_t num = send(sockfd, buf, len, 0);
      total += num;
      if (total == len) {
        break;
      }
    }
    return total;
  }

  ~Socket() {}
  // constructor and copy constructor
};

#endif
