#include "transaction.h"
#include <vector>

std::string reduce_balance(connection *C, int account_id, double change) {
  work query(*C);

  std::string response;
  std::string sql = "SELECT ACCOUNT.BALANCE FROM ACCOUNT WHERE (ACCOUNT_ID =" +
                    to_string(account_id) + ") FOR UPDATE;";
  // std::cout << "sql: " << sql << std::endl;
  result read(query.exec(sql));
  if (read.size() == 0) {
    response = "<error account_id = \"" + to_string(account_id) +
               ">No that account found</error>";
    return response;
  } else {
    result::const_iterator it = read.begin();
    double current_balance = it[0].as<double>();
    // std::cout << "current_balance: " << current_balance << std::endl;
    if (current_balance + change < 0) {
      response = "<error account_id = \"" + to_string(account_id) +
                 ">No enough balance in this account</error>";
      return response;
      query.commit();

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
int insert_open(connection *C, int account_id, std::string sym, int amount,
                double limit) {
  // simply insert data into transaction table first
  std::string sql = "INSERT INTO TRANSACTION(TRANS_ID) VALUES(DEFAULT);";
  work insert_trans(*C);
  result read_trans(insert_trans.exec(sql));
  insert_trans.commit();

  // Then insert data into open table
  sql = "INSERT INTO OPEN(SYM,LIMI,SHARES,ACCOUNT_ID) VALUES(\'" + sym + "\'," +
        to_string(limit) + "," + to_string(amount) + "," +
        to_string(account_id) + ") RETURNING TRANS_ID;";
  work insert(*C);
  result read(insert.exec(sql));
  insert.commit();
  return read.begin()[0].as<int>();
}
void insert_executed(connection *C, int trans_id, int deal_amount,
                     double exec_price) {
  time_t time_now = time(NULL);
  std::string sql = "INSERT INTO EXECUTED(PRICE,TRANS_ID,SHARES,TIME) VALUES(" +
                    to_string(exec_price) + "," + to_string(trans_id) + "," +
                    to_string(deal_amount) + "," + to_string(time_now) + ");";
  work executed(*C);
  executed.exec(sql);
  executed.commit();
}
void split_open(connection *C, int account_id, int new_amount, double new_price,
                result read, int trans_id) {
  int sign = new_amount > 0 ? 1 : -1; // 1: buy 2: sell
  cout << "read.size()" << read.size() << endl;
  for (result::const_iterator it = read.begin(); it != read.end(); ++it) {
    if (new_amount == 0) {
      break;
    }
    double wait_price = it[2].as<double>();
    int wait_amount = it[3].as<int>();
    int wait_acc_id = it[4].as<int>();
    std::string sym = it[1].as<string>();
    int wait_trans_id = it[0].as<int>();
    double exec_price = wait_price;
    work del(*C);
    std::string sql;
    int dealed_amount = min(abs(wait_amount), abs(new_amount));
    // new_amount = sign * (abs(new_amount) - dealed_amount);
    double dealed_money = dealed_amount * exec_price;
    double pre_paid_money = dealed_amount * max(new_price, wait_price);
    double return_money = pre_paid_money - dealed_money;

    int del_acc_id = new_amount < 0 ? account_id : wait_acc_id; // sell
    int edt_acc_id = new_amount > 0 ? account_id : wait_acc_id; // buy
    int sell_trans = new_amount < 0 ? trans_id : wait_trans_id;
    int buy_trans = new_amount > 0 ? trans_id : wait_trans_id;

    cout << "new_amount: " << new_amount << endl;
    cout << "wait_amount: " << wait_amount << endl;
    cout << "sell_trans: " << sell_trans << endl;
    cout << "buy_trans: " << buy_trans << endl;

    if (new_amount + wait_amount > 0) {
      // means buy.amount > sell.amount
      sql = "DELETE FROM OPEN WHERE TRANS_ID =" + to_string(sell_trans) + ";";
      del.exec(sql);
      sql = "UPDATE OPEN SET SHARES=" + to_string(new_amount + wait_amount) +
            " WHERE TRANS_ID=" + to_string(buy_trans) + ";";
      del.exec(sql);
      del.commit();

    } else if (new_amount + wait_amount < 0) {
      // means sell.amount > buy.amount
      sql = "DELETE FROM OPEN WHERE TRANS_ID =" + to_string(buy_trans) + ";";
      del.exec(sql);
      sql = "UPDATE OPEN SET SHARES=" + to_string(new_amount + wait_amount) +
            " WHERE TRANS_ID=" + to_string(sell_trans) + ";";
      del.exec(sql);
      del.commit();

    } else {
      // means sell.amount == buy.amount
      sql = "DELETE FROM OPEN WHERE TRANS_ID =" + to_string(buy_trans) + ";";

      del.exec(sql);
      sql = "DELETE FROM OPEN WHERE TRANS_ID =" + to_string(sell_trans) + ";";
      del.exec(sql);
      del.commit();
    }
    reduce_balance(C, del_acc_id, dealed_money);
    reduce_balance(C, edt_acc_id, return_money);

    cout << "edt_acc_id: " << edt_acc_id << endl;
    create_sym(C, sym, edt_acc_id, dealed_amount);

    insert_executed(C, sell_trans, dealed_amount, exec_price);
    insert_executed(C, buy_trans, dealed_amount, exec_price);
    new_amount = sign * (abs(new_amount) - dealed_amount);
  }
}
std::string pair_order(connection *C, int account_id, std::string sym,
                       int amount, double limit) {
  std::string sql = "", response = "";

  work query(*C);
  if (amount > 0) {
    // it is buy ,check all opened order amount < 0
    // put executed into EXECUTE
    sql = "SELECT * FROM OPEN WHERE (SHARES < 0 AND SYM = '" + sym +
          "') ORDER BY LIMI ASC, TRANS_ID ASC FOR UPDATE;";
    result read(query.exec(sql));
    // query.commit();
    // cout << read.size() << endl;
    if (read.size() != 0 && read.begin()[2].as<double>() <= limit) {
      int trans_id = insert_open(C, account_id, sym, amount, limit);
      response = "<opened sym=\"" + sym + "\" amount=\"" + to_string(amount) +
                 "\" limit=\"" + to_string(limit) + "\" id=\"" +
                 to_string(trans_id) + "\"/>";
      split_open(C, account_id, amount, limit, read, trans_id);

      return response;

    } else {
      int trans_id = insert_open(C, account_id, sym, amount, limit);
      // cout << "trans_id: " << trans_id << endl;
      response = "<opened sym=\"" + sym + "\" amount=\"" + to_string(amount) +
                 "\" limit=\"" + to_string(limit) + "\" id=\"" +
                 to_string(trans_id) + "\"/>";
      query.commit();
      return response;
    }
  } else if (amount < 0) {
    // it is sell,check all opened order amount > 0
    sql = "SELECT * FROM OPEN WHERE (SHARES > 0 AND SYM = '" + sym +
          "') ORDER BY LIMI DESC, TRANS_ID ASC FOR UPDATE;";
    result read(query.exec(sql));
    cout << "sql: " << sql << endl;
    cout << "limit: " << limit << endl;
    cout << "amount: " << amount << endl;
    // query.commit();
    cout << read.begin()[0].as<int>() << endl;
    cout << read.begin()[1].as<string>() << endl;
    cout << read.begin()[2].as<double>() << endl;
    cout << read.begin()[3].as<int>() << endl;
    cout << read.begin()[4].as<int>() << endl;
    // try to select the biggest buy order
    if (read.size() != 0 && read.begin()[2].as<double>() >= limit) {
      int trans_id = insert_open(C, account_id, sym, amount, limit);
      response = "<opened sym=\"" + sym + "\" amount=\"" + to_string(amount) +
                 "\" limit=\"" + to_string(limit) + "\" id=\"" +
                 to_string(trans_id) + "\"/>";
      cout << "TRAAAAAAAANS ID: " << trans_id << endl;
      split_open(C, account_id, amount, limit, read, trans_id);
      return response;
    } else {
      cout << "HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
      int trans_id = insert_open(C, account_id, sym, amount, limit);
      response = "<opened sym=\"" + sym + "\" amount=\"" + to_string(amount) +
                 "\" limit=\"" + to_string(limit) + "\" id=\"" +
                 to_string(trans_id) + "\"/>";
      query.commit();
      return response;
    }
  } else {
    response = "<error id =\"" + to_string(account_id) +
               "\">Order amount should not be zero</error>";
    return response;
    // amount = 0 is not allowed.
    // put executed into EXECUTE
  }
}
bool check_balance(connection *C, int account_id, double money_need) {

  // std::cout << "money_need: " << money_need << std::endl;

  return reduce_balance(C, account_id, 0 - money_need) == "succeed";
}

bool check_storage(connection *C, int account_id, int amount_need,
                   std::string sym) {
  // amount_need should be negative

  std::string sql = "SELECT * FROM SYM WHERE (SYM = \'" + sym +
                    "\' AND ACCOUNT_ID = '" + to_string(account_id) +
                    "')FOR UPDATE;";
  // cout << "sql: " << sql << endl;
  work query(*C);
  result read(query.exec(sql));
  if (read.size() == 0)
    query.commit();
  return false;
  double current_amount = read.begin()[2].as<int>();
  // cout << "sym_name: " << sym << endl;
  // cout << "current_amount: " << current_amount << endl;
  // cout << "amount_need: " << amount_need << endl;
  if (current_amount + amount_need < 0)
    return false;
  sql = "UPDATE SYM SET NUM = " + to_string(current_amount + amount_need) +
        " WHERE ACCOUNT_ID = " + to_string(account_id) + " AND SYM = '" + sym +
        "';";
  query.exec(sql);
  query.commit();
  return true;
}

std::string confirm_order(connection *C, int account_id, std::string sym,
                          int amount, double limit) {
  std::string response = "succeed";
  std::cout << "amount: " << amount << std::endl;
  std::cout << "limit: " << limit << std::endl;
  if (amount > 0) { // buy
    if (!check_balance(C, account_id, amount * limit))
      response = "<error account_id=\"" + to_string(account_id) +
                 "\">No enough balance in this account</error> ";
  } else if (amount < 0) { // sell
    if (!check_storage(C, account_id, amount, sym))
      response = "<error account_id=\"" + to_string(account_id) +
                 "\">No enough position in this account</error>";
  } else {
    response = "<error account_id=\"" + to_string(account_id) +
               "\">order's amount should not be zero</error>";
  }
  return response;
}

std::string insert_order(connection *C, int account_id, std::string sym,
                         int amount, double limit) {
  std::string response;
  if (!check_acc_id(C, account_id)) {
    response = "<error sym=\"" + sym + "\" amount=\"" + to_string(amount) +
               "\" limit=\"" + to_string(limit) +
               "\">invalid account id</error>";
    return response;
  }
  if ((response = confirm_order(C, account_id, sym, amount, limit)) !=
      "succeed")
    return response;
  // time_record

  return pair_order(C, account_id, sym, amount, limit);
}

std::vector<std::string> cancel_trans(connection *C, int trans_id) {

  std::string sql =
      "SELECT * FROM OPEN WHERE TRANS_ID =" + to_string(trans_id) +
      " FOR UPDATE; ";
  work query(*C); // select all order with trans_id in OPENED and cancel it
  result read(query.exec(sql));
  std::vector<std::string> responses;
  responses.push_back("<canceled id=\"" + to_string(trans_id) + "\">");
  time_t now_time = time(NULL);
  query.commit();

  work trans(*C);
  for (result::const_iterator it = read.begin(); it != read.end(); ++it) {

    std::string temp = "<canceled shares=\"" + to_string(it[3].as<int>()) +
                       "\" time=\"" + to_string(now_time) + "\"/>";
    responses.push_back(temp);
    sql = "INSERT INTO CANCELED(TRANS_ID,SHARES,TIME,ACCOUNT_ID) VALUES(" +
          to_string(trans_id) + "," + to_string(it[3].as<int>()) + "," +
          to_string(now_time) + " ," + to_string(it[4].as<int>()) + ");";
    trans.exec(sql);
  }
  trans.commit();
  sql = "DELETE FROM OPEN WHERE TRANS_ID =" + to_string(trans_id) + ";";
  work del(*C);
  del.exec(sql); // delete from open in the end
  del.commit();

  sql = "SELECT * FROM EXECUTED WHERE TRANS_ID=" + to_string(trans_id) + ";";
  work executed_read(*C);
  result read2(executed_read.exec(sql));
  executed_read.commit();
  for (result::const_iterator it = read2.begin(); it != read2.end(); ++it) {
    std::string temp = "<executed shares=\"" + to_string(it[2].as<int>()) +
                       "\" price=\"" + to_string(it[0].as<double>()) +
                       "\" time=\"" + to_string(it[3].as<long>()) + "\"/>";
    responses.push_back(temp);
  }

  responses.push_back("</canceled>");
  return responses;
}

std::vector<std::string> query_trans(connection *C, int trans_id) {

  std::vector<string> responses;
  responses.push_back("<status id=\"" + to_string(trans_id) + "\">");
  std::string sql =
      "SELECT * FROM OPEN WHERE TRANS_ID=" + to_string(trans_id) + ";";
  work open_read(*C);
  result read(open_read.exec(sql));
  open_read.commit();
  for (result::const_iterator it = read.begin(); it != read.end(); ++it) {
    std::string temp = "<open shares=\"" + to_string(it[3].as<int>()) + "\"/>";
    responses.push_back(temp);
  }
  sql = "SELECT * FROM CANCELED WHERE TRANS_ID=" + to_string(trans_id) + ";";
  work cancel_read(*C);
  result read2(cancel_read.exec(sql));
  cancel_read.commit();
  for (result::const_iterator it = read2.begin(); it != read2.end(); ++it) {
    std::string temp = "<canceled shares=\"" + to_string(it[1].as<int>()) +
                       "\" time=\"" + to_string(it[2].as<int>()) + "\"/>";
    responses.push_back(temp);
  }
  sql = "SELECT * FROM EXECUTED WHERE TRANS_ID=" + to_string(trans_id) + ";";
  work executed_read(*C);
  result read3(executed_read.exec(sql));
  executed_read.commit();
  for (result::const_iterator it = read3.begin(); it != read3.end(); ++it) {
    std::string temp = "<executed shares=\"" + to_string(it[2].as<int>()) +
                       "\" price=\"" + to_string(it[0].as<double>()) +
                       "\" time=\"" + to_string(it[3].as<long>()) + "\"/>";
    responses.push_back(temp);
  }
  responses.push_back("</status>");
  return responses;
}
