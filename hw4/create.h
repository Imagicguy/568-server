#ifndef __CREATE_H__
#define __CREATE_H__
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;
std::string create_account(connection *C, int account_id, int balance);
bool check_acc_id(connection *C, int account_id);
std::string create_sym(connection *C, std::string sym, int account_id, int num);
#endif
