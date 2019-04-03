#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__
#include "create.h"
#include <time.h>
void insert_executed(connection *C, int trans_id, int deal_amount,
                     double exec_price);
int insert_open(connection *C, int account_id, std::string sym, int amount,
                double limit);
void split_open(connection *C, int account_id, int new_amount, double new_price,
                result read, int trans_id);
bool check_balance(connection *C, int account_id, double money_need);
bool check_storage(connection *C, int account_id, int amount_need,
                   std::string sym);
std::string confirm_order(connection *C, int account_id, std::string sym,
                          int amount, double limit);
std::string reduce_balance(connection *C, int account_id, double change);
std::string pair_order(connection *C, int account_id, std::string sym,
                       int amount, double limit);
std::string insert_order(connection *C, int account_id, std::string sym,
                         int amount, double limit);
std::vector<std::string> cancel_trans(connection *C, int trans_id);
std::vector<std::string> query_trans(connection *C, int trans_id);
void query_order(connection *C, int trans_id);

#endif
