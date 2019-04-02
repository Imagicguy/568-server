#include "create.h"

bool check_acc_id(connection *C, int account_id) {
  std::string sql =
      "SELECT * FROM ACCOUNT WHERE (ACCOUNT_ID =" + to_string(account_id) +
      " )";
  work R(*C);
  result read(R.exec(sql));
  if (read.begin() == read.end()) {

    return false;
  }

  return true;
}

std::string create_account(connection *C, int account_id, int balance) {

  std::string response;
  if (balance < 0) {
    response = "<error id= \"" + to_string(account_id) +
               "\">Balance can't be negative<//error> ";
    return response;
  }

  if (!check_acc_id(C, account_id)) {

    std::string order = "INSERT INTO ACCOUNT (ACCOUNT_ID,BALANCE) "
                        "VALUES (" +
                        to_string(account_id) + ", " + to_string(balance) +
                        ");";
    work R(*C);
    R.exec(order);
    R.commit();
    response = "<created id= \"" + to_string(account_id) + "\" > ";

    return response;
  } else {
    response = "<error id= \"" + to_string(account_id) +
               "\">Account already existed</error> ";

    return response;
  }
}

std::string create_sym(connection *C, std::string sym, int account_id,
                       int num) {
  std::string response;
  std::string sql = "";

  if (!check_acc_id(C, account_id)) {
    response = "<error id= \"" + to_string(account_id) + "\" SYM = \'" + sym +
               " \'>Account doesnt exist<//error> ";

    return response;
  }
  //  std::cout << "after check" << std::endl;
  work R(*C);
  sql = "SELECT * FROM SYM WHERE (ACCOUNT_ID =" + to_string(account_id) +
        " AND SYM = \'" + sym + "\' );";
  result read_sym(R.exec(sql));

  if (read_sym.begin() == read_sym.end()) {

    sql = "INSERT INTO SYM(ACCOUNT_ID,SYM,NUM) VALUES(" +
          to_string(account_id) + ",\'" + sym + "\'," + to_string(num) + ");";
    R.exec(sql);
    R.commit();
  } else {

    result::const_iterator it = read_sym.begin();

    int old_value = it[2].as<int>();

    num += old_value;

    sql = "UPDATE SYM SET NUM = " + to_string(num) + " WHERE SYM = \'" +
          it[1].as<string>() + "\';";
    R.exec(sql);
    R.commit();
  }
  response =
      "<created id= \"" + to_string(account_id) + "\" sym = \"" + sym + "\">";

  return response;
}
