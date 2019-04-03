#include "database.h"

void cleanTable(connection *C) {
  if (C->is_open()) {
    work dropTable(*C);

    dropTable.exec("DROP TABLE IF EXISTS CANCELED;");
    dropTable.exec("DROP TABLE IF EXISTS EXECUTED;");
    dropTable.exec("DROP TABLE IF EXISTS OPEN;");
    dropTable.exec("DROP TABLE IF EXISTS TRANSACTION;");
    dropTable.exec("DROP TABLE IF EXISTS SYM;");
    dropTable.exec("DROP TABLE IF EXISTS ACCOUNT;");

    dropTable.commit();

  } else {
    cout << "failed to clean table" << endl;
  }
}
void initTable(connection *C) {

  std::string accountTable, symTable, transactionTable, openTable,
      canceledTable, executedTable;
  accountTable = "CREATE TABLE ACCOUNT("
                 "ACCOUNT_ID    INT PRIMARY KEY              NOT NULL,"
                 "BALANCE       DOUBLE PRECISION             NOT NULL);";

  symTable = "CREATE TABLE SYM("
             "ACCOUNT_ID INT REFERENCES ACCOUNT(ACCOUNT_ID)  NOT NULL,"
             "SYM        TEXT                                NOT NULL,"
             "NUM        INT                                 NOT NULL);";

  transactionTable =
      "CREATE TABLE TRANSACTION("
      "TRANS_ID  SERIAL PRIMARY KEY                     NOT NULL);";

  openTable = "CREATE TABLE OPEN("
              "TRANS_ID   SERIAL REFERENCES TRANSACTION(TRANS_ID)    NOT NULL,"
              "SYM       TEXT                                NOT NULL,"
              "LIMI     DOUBLE PRECISION                    NOT NULL,"
              "SHARES    INT                                 NOT NULL,"
              "ACCOUNT_ID INT REFERENCES ACCOUNT(ACCOUNT_ID) NOT NULL);";
  canceledTable = "CREATE TABLE CANCELED("
                  "TRANS_ID INT REFERENCES TRANSACTION(TRANS_ID)    NOT NULL,"
                  "SHARES   INT                              NOT NULL,"
                  "TIME     INT                              NOT NULL,"
                  "ACCOUNT_ID INT REFERENCES ACCOUNT(ACCOUNT_ID) NOT NULL);";
  executedTable = "CREATE TABLE EXECUTED("
                  "PRICE    DOUBLE PRECISION                 NOT NULL,"
                  "TRANS_ID INT REFERENCES TRANSACTION(TRANS_ID)    NOT NULL,"
                  "SHARES   INT                              NOT NULL,"
                  "TIME     INT                              NOT NULL);";

  work createTable(*C);
  createTable.exec(accountTable);
  createTable.exec(symTable);
  createTable.exec(transactionTable);
  createTable.exec(openTable);
  createTable.exec(canceledTable);
  createTable.exec(executedTable);
  createTable.commit();
  cout << "initTable succeed!" << endl;
}
