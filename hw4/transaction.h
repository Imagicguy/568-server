#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__
#include "create.h"
#include <time.h>
void handle_order(connection *C);
void insert_open(connection *C, int account_id, std::string sym, int amount,
                 double limit);
void split_open(connection *C, int account_id, std::string sym, int amount,
                double limit, result read);
bool check_balance(connection *C, double money_need);
bool check_storage(connection *C, int amount_need);
std::string confirm_order(connection *C);
std::string reduce_balance(connection *C, int account_id);
std::string pair_order(connection *C);
std::string insert_order(connection *C);
std::vector<std::string> cancel_trans(connection *C, int trans_id);
std::vector<std::string> query_trans(connection *C, int trans_id);
void query_order(connection *C, int trans_id);

#endif
