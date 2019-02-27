#include "proxy.h"
#include "parse.h"
void process(int ID);
void error_check(int &ID, int error);
void set_client(const char *hostname, const char *port);

void Proxy::error_check(int &ID, int error) {
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
}

int Proxy::set_client(const char *hostname, const char *port) {
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *master_hostname = hostname;
  const char *master_port = port;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(master_hostname, master_port, &host_info, &host_info_list) !=
      0) {
    std::cerr << "Error: cannot get address info for host" << std::endl;
    std::cerr << "  (" << master_hostname << "," << master_port << ")"
              << std::endl;
    return -1;
  }
  int socket_fd;
  if ((socket_fd =
           socket(host_info_list->ai_family, host_info_list->ai_socktype,
                  host_info_list->ai_protocol)) == -1) {
    std::cerr << "Error: cannot create socket" << std::endl;
    std::cerr << "  (" << master_hostname << "," << master_port << ")"
              << std::endl;
    return -1;
  }

  if (connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) ==
      -1) {
    std::cerr << "Error: cannot connect to socket" << std::endl;
    std::cerr << "  (" << master_hostname << "," << master_port << ")"
              << std::endl;
    return -1;
  }
  return socket_fd;
}
void Proxy::process(int ID) {
  std::string content;
  int recv_bytes = 1;
  int capacity = 5000;

  char buffer[capacity];

  memset(buffer, 0, capacity);
  recv_bytes = recv(client_fd, buffer, capacity, 0);

  std::string request(buffer);
  std::string header = request.substr(0, request.find("\r\n"));
  if (recv_bytes < 0) {
    error_check(ID, 1);
    return;
  }

  if (header.find("POST") != std::string::npos) {
    // std::cout << "this is a post " << std::endl;
    // std::cout << request << std::endl;
    // post_handler();
  } else if ((header.find("CONNECT") != std::string::npos)) {
    Parse request_t;
    request_t.init_connect(request, header);
    connect_handler(request_t, ID);
  } else if ((header.find("GET") != std::string::npos)) {
    // std::cout << "this is a get" << std::endl;
    Parse request_t;
    request_t.init_get(request, header);
    get_handler(request_t, ID);
  } else {
    error_check(ID, 400);
  }
  content += std::string(buffer);
}

void Proxy::get_handler(Parse request_t, int ID) {
  std::cout << request_t.request << std::endl;
  const char *port = request_t.port.c_str();
  const char *host_name = request_t.hostname.c_str();
  int server_fd = set_client(host_name, port);
  int start1 = request_t.request.find("http://") + 7;
  std::string temp1 = request_t.request.substr(start1);
  int start2 = temp1.find("/ ");
  // request_t.request = request_t.request.substr(0, start1 - 7) +
  // request_t.request.substr(start2);

  request_t.request.replace(request_t.request.find("keep-alive"), 10, "closed");
  std::cout << "request is " << request_t.request << std::endl;
  if (send(server_fd, request_t.request.c_str(),
           strlen(request_t.request.c_str()), 0) < 0) {
    error_check(ID, 2);
    close(server_fd);
    return;
  }

  char buff[10000];

  /*std::cout << "1" << std::endl;
  while (len > 0 && (n = recv(server_fd, p, len, 0) > 0)) {
    std::cout << n << std::endl;
    p += n;
    len = len - (size_t)n;
  }
  std::cout << "-1" << std::endl;*/
  int max_len;
  while ((max_len = recv(server_fd, buff, 10000, 0)) > 0) {
    if (send(client_fd, buff, max_len, MSG_NOSIGNAL) < 0) {
      error_check(ID, 2);
      close(server_fd);
      return;
    }
  }
}
void Proxy::connect_handler(Parse request_t, int ID) {
  const char *port = request_t.port.c_str();
  const char *host_name = request_t.hostname.c_str();
  int server_fd = set_client(host_name, port);
  std::string msg = "HTTP/1.1 200 OK \r\n\r\n";
  if (send(client_fd, msg.c_str(), strlen(msg.c_str()), 0) < 0) {
    error_check(ID, 2);
    close(server_fd);
    return;
  }
  int max = (server_fd > client_fd) ? server_fd : client_fd;
  max++;
  fd_set fds;
  char buffer[50000];
  memset(buffer, 0, 50000);
  struct timeval tv;
  tv.tv_sec = 10;
  tv.tv_usec = 500000;
  int rv;
  while (1) {
    FD_ZERO(&fds);
    FD_SET(server_fd, &fds);
    FD_SET(client_fd, &fds);
    memset(buffer, 0, 50000);
    if ((rv = select(max, &fds, NULL, NULL, &tv)) == -1) {
      error_check(ID, 3);
      return;
    } else if (rv == 0) {
      //      std::cout << "Timeout occurred! No data after 10.5 seconds" <<
      //      std::endl;
    } else {
      int recv_bytes;
      int send_bytes;
      if (FD_ISSET(server_fd, &fds) > 0) {
        if ((recv_bytes = recv(server_fd, buffer, 50000, 0)) < 0) {
          error_check(ID, 1);
          break;
        } else if (recv_bytes == 0) {
          close(server_fd);
          return;
        }
        if ((send_bytes = send(client_fd, buffer, recv_bytes, 0)) < 0) {
          error_check(ID, 2);

          break;
        }
      } else {
        if ((recv_bytes = recv(client_fd, buffer, 50000, 0)) < 0) {
          error_check(ID, 1);
          break;
        } else if (recv_bytes == 0) {
          close(server_fd);
          return;
        }
        if ((send_bytes = send(server_fd, buffer, recv_bytes, 0)) < 0) {
          error_check(ID, 2);

          break;
        }
      }
    }
  }
  close(server_fd);
  return;
}
void Proxy::post_handler(Parse request_t, int ID){};
