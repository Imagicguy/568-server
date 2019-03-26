#include "database.h"
#include "XML.h"

void generate_response() {}

void handle_order(connection *C, XML xml) {}

void create_account(connection *C, XML xml) {
  // account_id ,balance
  work query(*C);
  std::string response;
  std::string sql =
      "SELECT * FROM ACCOUNT WHERE (ACCOUNT_ID ==" + to_string(account_id) +
      ");";
  result read(query.exec(sql));
  if (read == NULL) {
    std::string order = "INSERT INTO ACCOUNT (ACCOUNT_ID,BALANCE) "
                        "VALUES (" +
                        to_string(account_id) + ", " + to_string(balance) +
                        ");";
    response = "<created id=\""
               ";
        generate_response();
  } else {
    response = "<error id=\"" + to_string(account_id) +
               "\">Account already existed<//error> ";
  }
}

void change_balance(connection *C, XML xml) {}
void create_sym(connection *C, XML xml) {
  std::string sym;
  generate_response();
}

void insert_order(connection *C, XML xml) {
  generate_response();
  handle_order(C, xml);
}

void query_order(connection *C, int trans_id) { generate_response(); }

void resetTable(connection *C) {
  if (C->is_open()) {
    work dropTable(*C);
    dropTable.exec("DROP TABLE IF EXISTS ACCOUNT;");
    dropTable.exec("DROP TABLE IF EXISTS SYM;");
    dropTable.exec("DROP TABLE IF EXISTS OPEN;");
    dropTable.exec("DROP TABLE IF EXISTS CANCELED;");
    dropTable.exec("DROP TABLE IF EXISTS EXECUTED;");
    dropTable.commit();
    cout << "resetTable succeed!" << endl;
  }
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
  cout << "createTable succeed!" << endl;
}
