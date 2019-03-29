#include "transaction.h"

void handle_order(connection *C) {}

std::string reduce_balance(connection *C, int account_id, double change) {
  work query(*C);

  std::string response;
  std::string sql = "SELECT ACCOUNT.BALANCE FROM ACCOUNT WHERE (ACCOUNT_ID =" +
                    to_string(account_id) + ");";
  result read(query.exec(sql));
  if (read.size() == 0) {
    response = "<error account_id = \"" + to_string(account_id) +
               ">No that account found</error>";
    return response;
  } else {
    result::const_iterator it = read.begin();
    double current_balance = it[0].as<int>();
    if (current_balance + change < 0) {
      response = "<error account_id = \"" + to_string(account_id) +
                 ">No enough balance in this account</error>";
      return response;

    } else {
      sql = "UPDATE ACCOUNT SET BALANCE = " +
            to_string(current_balance + change) +
            " WHERE ACCOUNT_ID = " + to_string(account_id) + ";";
      query.exec(sql);
      query.commit();
      response = "succeed";
      return response;
    }
  }
}

std::string pair_order(connection *C) {
  int account_id = 0;
  std::string sym = "", sql = "", response = "";
  double amount = 0;
  double limit = 0;
  if (amount > 0) {
    // it is buy ,check all opened order amount < 0
    // put executed into EXECUTE
  } else if (amount < 0) {
    // it is sell,check all opened order amount > 0
  } else {
    // amount = 0 is not allowed.
    // put executed into EXECUTE
  }
}
bool check_balance(connection *C, double money_need) {
  int account_id = 0;

  return reduce_balance(C, account_id, money_need) == "succeed";
}

bool check_storage(connection *C, int amount_need) {
  // amount_need should be negative
  int account_id = 0;
  std::string sym = "";
  std::string sql = "SELECT * FROM SYM WHERE (SYM = \'" + sym +
                    "\' AND ACCOUNT_ID = " + to_string(account_id) + "');";
  work query(*C);
  result read(query.exec(sql));
  if (read.size() == 0)
    return false;
  double current_amount = read.begin()[0].as<double>();
  if (current_amount < amount_need)
    return false;
  sql = "UPDATE SYM SET AMOUNT = " + to_string(current_amount + amount_need) +
        " WHERE ACCOUNT_ID = " + to_string(account_id) + ";";
  query.exec(sql);
  query.commit();
  return true;
}

std::string confirm_order(connection *C) {
  double amount = 0, limit = 0;
  int account_id = 0;
  std::string response = "succeed";
  if (amount > 0) { // buy
    if (check_balance(C, amount * limit))
      return response;
  } else if (amount < 0) { // sell
    if (check_storage(C, amount))
      return response;
  } else {
    response = "<error account_id = \"" + to_string(account_id) +
               "\">order's amount should not be zero </error> ";
    return response;
  }
}
std::string insert_order(connection *C) {
  int account_id = 0;
  std::string sym = "";
  std::string response;
  double amount = 0;
  double limit = 0;
  if (!check_acc_id(C, account_id)) {
    response = "<error sym= \"" + sym + "\" amount= \"" + to_string(amount) +
               "\" limit = \"" + to_string(limit) +
               "\">invalid account id</error>";
    return response;
  }
  if ((response = confirm_order(C)) != "succeed")
    return response;
  // time_record
  pair_order();
}
void cancel_trans(connection *C, int trans_id) {
  // select all order with trans_id in OPENED and cancel it
}
void query_order(connection *C, int trans_id) {
  // select all order with trans_id in OPENED,EXECUTED and CANCELED and return
  // it
  // maybe need a for loop to execute this part
}
