#ifndef __RESPONSE__H__
#define __RESPONSE__H__
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class finalResponse {
public:
  bool revalidation;
  bool proxyrev;
  bool nocache;
  bool nostore;
  bool notransform;
  bool pub;
  bool priv;
  string ETag;
  time_t maxage;
  // time_t smaxage;
  int Age;
  string data;
  finalResponse()
      : revalidation(false), proxyrev(false), nocache(false), nostore(false),
        notransform(false), pub(true), priv(false), maxage(0) {}
  ~finalResponse() {}
};

#endif
