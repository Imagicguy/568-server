#ifndef __PRASE_H__
#define __PRASE_H__
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
class Parse {
public:
  std::string request;
  std::string req_line;
  std::string type;
  std::string hostname;
  std::string port;
  std::string header;
  std::string age;

  Parse() {}
  void init_connect(std::string request, std::string line);
  void init_get(std::string request, std::string line);
  void parse_type(std::string line);
  void parse_hostname_c(std::string line);
  void parse_port_c(std::string line);
  void parse_hostname_g(std::string line);
};
#endif
