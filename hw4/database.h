#ifndef __DATABASE_H__
#define __DATABASE_H__
#include "XML.h"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

void resetTable(connection *C);
std::string create_account(connection *C, XML xml);
std::string create_sym(connection *C);

// void insertTable(connection *C);
#endif
