// using namespace std;
#include "parse.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <time.h>
#include <unordered_map>
#include <vector>
using namespace std;
class Cache {
private:
  string request;
  int id;
  string response;

  int Age; // header field
  string expiretime;
  string ETag;
  int size; // size of response received
  int fd;

public:
  Cache() {}
  bool correct_status();

  string get_expire();

  string get_maxage();

  string get_date();
  string control_sig();

  string get_ETag();

  string get_modified();
  void init_cache(std::string request_input);
  void savecache(unordered_map<string, finalResponse> cache);
  bool checkcache(unordered_map<string, finalResponse> cache);
};
