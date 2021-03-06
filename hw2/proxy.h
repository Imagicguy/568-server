#ifndef __PROXY_H__
#define __PROXY_H__
#include "cache.h"
#include "main.h"
#include "parse.h"
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
  int proxy_fd;
  std::string ip;
  const char *port_num;

public:
  Proxy(int proxy, int client, std::string ipAdd, const char *num)
      : client_fd(client), proxy_fd(proxy), ip(ipAdd), port_num(num) {}
  void error_check(int &ID, int error);
  int set_client(const char *hostname, const char *port);
  void get_handler(Parse request_t, int ID); // need cache
  void post_handler(Parse request_t, int ID);
  void connect_handler(Parse request_t, int ID);
  void process(int ID);
};
#endif
