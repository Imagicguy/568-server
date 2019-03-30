#ifndef __XML_PARSE_H__
#define __XML_PARSE_H__
#include "create.h"
#include "tinyxml2.h"
#include "transaction.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;
using namespace tinyxml2;

struct Account {
  int id;
  int balance;
};

struct Symbol {
  string sym;
  int id;
  int num;
};

struct Order {
  string sym;
  int amount;
  double limit;
};

struct Query {
  int id;
};

struct Cancel {
  int id;
};

string resp_str(connection *C, const char *pSource);

#endif
