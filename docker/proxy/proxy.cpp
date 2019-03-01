#include "proxy.h"
#include "main.h"
#include "parse.h"
extern std::unordered_map<std::string, finalResponse> cache;

void process(int ID);
void error_check(int &ID, int error);
void set_client(const char *hostname, const char *port);

void Proxy::error_check(int &ID, int error) {
  std::ofstream file;

  file.open("proxy.log", std::ios::app);
  switch (error) {
  case 1: {
    /*file << ID << " : "
         << "ERROR recv() failed!" << std::endl;
         file.close();*/
    break;
  }

  case 2: {
    /*file << ID << " : "
         << "ERROR send() failed!" << std::endl;
         file.close();*/
    break;
  }

  case 3: {
    /*file << ID << " : "
         << "ERROR select() failed!" << std::endl;
         file.close();*/
    break;
  }

  case 503: {
    file << ID << " : "
         << "ERROR Service Unavailable" << std::endl;
    file.close();
    break;
  }

  case 400: {
    file << ID << " : "
         << "ERROR BAD REQUEST" << std::endl;
    file.close();
    break;
  }
  case 404: {
    file << ID << " : "
         << "ERROR NOT FOUND" << std::endl;
    file.close();
    break;
  }
  case 411: {
    file << ID << " : "
         << "ERROR LENGTH REQUIRED" << std::endl;
    file.close();
    break;
  }
  case 502: {
    file << ID << " : "
         << "ERROR BAD GATEWAY" << std::endl;
    file.close();
    break;
  }
  default: {
    file.close();
    return;
  }
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
    int ID = 80;
    error_check(ID, 6);
    return -1;
  }
  return socket_fd;
}
void Proxy::process(int ID) {
  std::string content;
  int recv_bytes = 1;
  char buffer[5000];
  int max_len = 1;
  size_t break_point = 0;
  char *p = buffer;
  memset(buffer, 0, 5000);
  do {
    max_len = recv(client_fd, p, 5000, 0);
    p += max_len;
    std::string temp(buffer);
    if ((break_point = temp.find("\r\n\r\n")) != std::string::npos) {
      break;
    }

  } while (max_len > 0);
  std::string temp2(buffer);
  if (temp2.find("Content-Length") != std::string::npos) {

    int start = temp2.find("Content-Length: ") + 16;
    std::string temp3 = temp2.substr(start);
    temp3 = temp3.substr(0, temp2.find("\r\n"));
    int length = atoi(temp3.c_str());
    if ((max_len = recv(client_fd, p, length, 0)) < 0) {
      error_check(ID, 1);
    }
  } else if (temp2.find("Transfer-Encoding") != std::string::npos) {
    // std::cout << "found chunked is : " << temp2.find("chunked") << std::endl;
    // std::cout << temp2 << std::endl;
    std::cout << "this is chunked!" << std::endl;
    do {
      max_len = recv(client_fd, p, 5000, 0);
      p += max_len;
      std::string temp(buffer);
      temp = temp.substr(temp.find("\r\n\r\n") + 8);

      if (temp.find("\r\n\r\n") != std::string::npos) {
        break;
      }
    } while (max_len > 0);
  } else {
  }

  std::string request(buffer);
  std::string header = request.substr(0, request.find("\r\n"));
  if (recv_bytes < 0) {
    error_check(ID, 1);
    return;
  }

  if (header.find("POST") != std::string::npos) {
    //  std::cout << "this is a post " << std::endl;
    // std::cout << request << std::endl;
    Parse request_t;
    request_t.init_get(request, header);
    post_handler(request_t, ID);
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
  // std::cout << request_t.request << std::endl;

  const char *port = request_t.port.c_str();
  const char *host_name = request_t.hostname.c_str();
  int server_fd = set_client(host_name, port);
  Cache block;
  // block.init_checkcache(request_t.request, ID, client_fd, server_fd, cache);
  // block.checkcache();

  request_t.request.replace(request_t.request.find("keep-alive"), 10, "closed");
  // std::cout << "request is " << request_t.request << std::endl;

  if (send(server_fd, request_t.request.c_str(),
           strlen(request_t.request.c_str()), 0) < 0) {
    error_check(ID, 2);
    close(server_fd);
    return;
  }

  std::ofstream file;
  file.open("/var/log/erss/proxy.log", std::ios::app);
  file << ID << ": Requesting \"" << request_t.header << "\" from "
       << request_t.hostname << std::endl;
  file.close();
  char buff[10000];
  int max_len;
  std::string response = "";
  std::string response_line = "";
  while ((max_len = recv(server_fd, buff, 10000, 0)) > 0) {
    std::string temp5(buff);
    response_line = response.substr(0, response.find("\r\n"));
    // std::cout << "temp5********" << std::endl;
    //  std::cout << temp5 << std::endl;
    temp5 = temp5.substr(0, max_len);
    response += temp5;
    if (send(client_fd, buff, max_len, MSG_NOSIGNAL) < 0) {
      error_check(ID, 2);
      close(server_fd);

      return;
    }
  }
  // std::cout << "response  is " << response << std::endl;

  // Cache block1;
  block.init_savecache(response_line, request_t.request, ID, cache);
  block.savecache();
  send(client_fd, response_line.c_str(), strlen(response_line.c_str()), 0);
  // std::string age = "";
  // std::string expires = "";
  // std::string cache_control = "";
  // if (response.find("Age: ") != std::string::npos) {
  // age = response.substr(response.find("Age: ") + 5);
  // age = age.substr(0, age.find("\r\n"));
  //}
  // if (response.find("Expires: ") != std::string::npos) {
  // expires = response.substr(response.find("Expires: ") + 9);
  // expires = expires.substr(0, expires.find("\r\n"));
  //}
  // if (response.find("Cache-control: ") != std::string::npos) {
  // cache_control = response.substr(response.find("Cache-control: ") + 15);
  // cache_control = cache_control.substr(0, cache_control.find("\r\n"));
  //}
  // std::cout << "Age: " << age << std::endl;
  // std::cout << "Expires: " << expires << std::endl;
  // std::cout << "Cache_control: " << cache_control << std::endl;
}
void Proxy::connect_handler(Parse request_t, int ID) {
  const char *port = request_t.port.c_str();
  const char *host_name = request_t.hostname.c_str();
  int server_fd = set_client(host_name, port);

  std::ofstream file;
  file.open("/var/log/erss/proxy.log", std::ios::app);
  file << ID << "Responding \"HTTP/1.1 200 OK\" " << std::endl;
  file.close();

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
void Proxy::post_handler(Parse request_t, int ID) {
  std::string request = request_t.request;
  int start = request.find("Content-Length: ") + 16;
  std::string temp = request.substr(start);
  temp = temp.substr(0, temp.find("\r\n"));
  // int length = atoi(temp.c_str());
  // std::cout << " is that correct length ? " << length << std::endl;

  // std::cout << request_t.request << std::endl;
  const char *port = request_t.port.c_str();
  const char *host_name = request_t.hostname.c_str();
  int server_fd = set_client(host_name, port);

  request_t.request.replace(request_t.request.find("keep-alive"), 10, "closed");
  // std::cout << "request is " << request_t.request << std::endl;
  if (send(server_fd, request_t.request.c_str(),
           strlen(request_t.request.c_str()), 0) < 0) {
    error_check(ID, 2);
    close(server_fd);
    return;
  }

  char buff[10000];
  int max_len;

  while ((max_len = recv(server_fd, buff, 10000, 0)) > 0) {
    if (send(client_fd, buff, max_len, MSG_NOSIGNAL) < 0) {
      error_check(ID, 2);
      close(server_fd);
      return;
    }
  }
}
