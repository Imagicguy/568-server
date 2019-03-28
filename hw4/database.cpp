#include "database.h"
#include "XML.h"

void handle_order(connection *C, XML xml) {}

std::string create_account(connection *C, XML xml) {
  // account_id ,balance
  work query(*C);
  int account_id = 0;
  int balance = 0;
  std::string response;
  if (balance < 0) {
    response = "<error id= \"" + to_string(account_id) +
               "\">Balance can't be negative<//error> ";
    return response;
  }
  std::string sql =
      "SELECT * FROM ACCOUNT WHERE (ACCOUNT_ID =" + to_string(account_id) +
      ");";
  nontransaction R(*C);
  result read(R.exec(sql));
  if (read.empty()) {
    std::string order = "INSERT INTO ACCOUNT (ACCOUNT_ID,BALANCE) "
                        "VALUES (" +
                        to_string(account_id) + ", " + to_string(balance) +
                        ");";
    response = "<created id= \"" + to_string(account_id) + "\" > ";
    return response;
  } else {
    response = "<error id= \"" + to_string(account_id) +
               "\">Account already existed<//error> ";
    return response;
  }
}

std::string change_balance(connection *C, int account_id) {
  work query(*C);

  int change = -100;
  std::string response;
  std::string sql = "SELECT *.BALANCE FROM ACCOUNT WHERE (ACCOUNT_ID =" +
                    to_string(account_id) + ");";
  result read(query.exec(sql));
  if (read.size() == 0) {
    response = "<error account_id = \"" + to_string(account_id) +
               ">No that account found</error>";
    return response;
  } else {
    result::const_iterator it = read.begin();
    int current_balance = it[0].as<int>();
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

std::string create_sym(connection *C, XML xml) {

  std::string sym = "";
  std::string response;
  int account_id = 0;
  int num = 0;
  std::string sql =
      "SELECT * FROM SYM WHERE (ACCOUNT_ID =" + to_string(account_id) + " )";
  work R(*C);
  result read(R.exec(sql));
  if (read.empty()) {
    response = "<error id= \"" + to_string(account_id) + "\" SYM = \'" + sym +
               " \'>Account doesnt exist<//error> ";
    return response;
  }
  sql = "SELECT * FROM SYM WHERE (ACCOUNT_ID =" + to_string(account_id) +
        " AND SYM = \'" + sym + "\' )";
  result read_sym(R.exec(sql));
  if (read_sym.empty()) {
    sql = "INSERT INTO SYM(ACCOUNT_ID,SYM,NUM) VALUES(" +
          to_string(account_id) + ",\'" + sym + "\'," + to_string(num) + ");";
    R.exec(sql);
    R.commit();
  } else {
    result::const_iterator it = read.begin();
    it++;
    num += it[1].as<int>();
    sql = "UPDATE SYM SET SYM = " + to_string(num) + " WHERE SYM = \'" +
          to_string(it[1].as<int>()) + "\';";
    R.exec(sql);
    R.commit();
  }
  response =
      "<created id= \"" + to_string(account_id) + "\" sym = \"" + sym + "\">";
  return response;
}
bool check_acc_id(connection *C, int account_id) {}
std::string insert_order(connection *C, XML xml) {
  int account_id = 0;
  std::string sym = "";
  std::string response;
  double amount = 0;
  double limit = 0;
  if (check_acc_id(C, account_id)) {
    response = change_balance(C, account_id);
    if (response == "succeed") {
      handle_order(C);
    } else {
      return response;
    }
  } else {
    response = "<error sym= \"" + sym + "\" amount= \"" + to_string(amount) +
               "\" limit = \"" + to_string(limit) +
               "\">invalid account id</error>";
    return response;
  }
}

void query_order(connection *C, int trans_id) {}

void cleanTable(connection *C) {
  if (C->is_open()) {
    work dropTable(*C);
    dropTable.exec("DROP TABLE IF EXISTS ACCOUNT;");
    dropTable.exec("DROP TABLE IF EXISTS SYM;");
    dropTable.exec("DROP TABLE IF EXISTS OPEN;");
    dropTable.exec("DROP TABLE IF EXISTS CANCELED;");
    dropTable.exec("DROP TABLE IF EXISTS EXECUTED;");
    dropTable.commit();

  } else {
    cout << "failed to clean table" << endl;
  }
}
void initTable(connection *C) {

  std::string accountTable, symTable, openTable, canceledTable, executedTable;
  accountTable = "CREATE TABLE ACCOUNT("
                 "ACCOUNT_ID    INT PRIMARY KEY              NOT NULL,"
                 "BALANCE       INT                          NOT NULL);";

  symTable = "CREATE TABLE SYM("
             "ACCOUNT_ID INT REFERENCES ACCOUNT(ACCOUNT_ID)  NOT NULL,"
             "SYM        TEXT                                NOT NULL,"
             "NUM        INT                                 NOT NULL);";

  openTable = "CREATE TABLE OPEN("
              "TRANS_ID  INT PRIMARY KEY                     NOT NULL,"
              "SYM       TEXT                                NOT NULL,"
              "AMOUNT    INT                                 NOT NULL,"
              "LIMIT     INT                                 NOT NULL,"
              "SHARES    INT                                 NOT NULL);";
  canceledTable = "CREATE TABLE CANCELED("
                  "TRANS_ID INT REFERENCES OPEN(TRANS_ID)    NOT NULL,"
                  "SHARES   INT                              NOT NULL,"
                  "TIME     INT                              NOT NULL);";
  executedTable = "CREATE TABLE EXECUTED("
                  "PRICE    INT                              NOT NULL,"
                  "SHARES   INT                              NOT NULL,"
                  "TIME     INT                              NOT NULL);";

  work createTable(*C);
  createTable.exec(accountTable);
  createTable.exec(symTable);
  createTable.exec(openTable);
  createTable.exec(canceledTable);
  createTable.exec(executedTable);
  createTable.commit();
  cout << "initTable succeed!" << endl;
}
