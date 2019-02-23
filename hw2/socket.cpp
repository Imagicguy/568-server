#include "socket.h"
int Socket::init_server() {
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

int Socket::init_client() {
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

ssize_t Socket::send_all(int sockfd, const void *buf, size_t len) {
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
