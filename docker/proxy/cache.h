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
  // int Age; // header field
  // string expiretime;
  // string ETag;
  int serverfd;
  int clientfd;
  unordered_map<string, finalResponse> cache;

public:
  Cache() {}
  bool correct_status(string response);

  string get_expire(string response);

  string get_maxage(string response);

  string control_sig(string response);

  string get_ETag(string response);

  string get_Age(string response);

  string get_modified(string response);

  string get_firstline(string request);

  string get_Host(string request);
  void init_checkcache(string request_input, int id, int clientfd_input,
                       int serverfd_input,
                       unordered_map<string, finalResponse> cache);
  void init_savecache(string response_input, string request_input, int id_input,
                      unordered_map<string, finalResponse> cache);
  void savecache();
  void checkcache();
};
