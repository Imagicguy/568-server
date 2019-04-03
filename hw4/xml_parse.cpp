#include "xml_parse.h"

string resp_str(connection *C, const char *pSource) {
  /* string pSource =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?><create><account id=\"123\" "
      "balance=\"1000\"/><account id=\"456\" balance=\"2000\"/><account "
      "id=\"123\" balance=\"3000\"/><symbol sym=\"SPY\"><account "
      "id=\"123\">100000</account></symbol><symbol sym=\"BTC\"><account "
      "id=\"456\">90000</account><account "
      "id=\"789\">70000</account></symbol><symbol sym=\"T5asdf\"><account "
      "id=\"789\">80000</account></symbol><symbol sym=\"SPY\"><account "
      "id=\"123\">70000</account><account "
      "id=\"789\">60000</account></symbol></create>";
  */
  string res;

  int len = strlen(pSource);
  cout << len << endl;
  XMLDocument xml;
  xml.Parse(pSource);
  xml.Print();
  // xml.LoadFile("test_create2.xml");

  XMLNode *RootNode = xml.FirstChild();
  if (RootNode == nullptr) {
    cout << "error" << endl;
    // return XML_ERROR_FILE_READ_ERROR;
    exit(1);
  }

  XMLNode *TopNode = RootNode->NextSibling(); // Pointer to top node
  XMLElement *TopElmt = TopNode->ToElement(); // <create> or <transactions>
  if (TopElmt == nullptr) {
    cout << "error" << endl;
    exit(1);
  }

  string TopElmtName = TopElmt->Name(); // create or transactions
  // <create> case
  if (TopElmtName == "create") {

    for (XMLNode *ChildNode = TopNode->FirstChild(); ChildNode != nullptr;
         ChildNode = ChildNode->NextSibling()) {
      XMLElement *child = ChildNode->ToElement();
      string childname = child->Name();
      // <account> case
      if (childname == "account") {
        // Account tmp_account;
        const XMLAttribute *ChildAttr = child->FirstAttribute();
        assert(strcmp(ChildAttr->Name(), "id") == 0);
        int id = stoi(child->Attribute(ChildAttr->Name()));
        const XMLAttribute *ChildAttr2 = ChildAttr->Next();
        assert(strcmp(ChildAttr2->Name(), "balance") == 0);
        int balance = stoi(child->Attribute(ChildAttr2->Name()));
        // tmp_account.id = id;
        // tmp_account.balance = balance;
        // cout << tmp_account.id << " " << tmp_account.balance << endl;
        string create_account_resp = create_account(C, id, balance);
        // res += "\n" + create_account_resp;
        res += create_account_resp;
      }
      // <symbol> case
      else if (childname == "symbol") {
        // Symbol tmp_symbol;
        const XMLAttribute *ChildAttr = child->FirstAttribute();
        assert(strcmp(ChildAttr->Name(), "sym") == 0);
        string sym = child->Attribute(ChildAttr->Name());

        for (XMLNode *ChildNode_InSym = ChildNode->FirstChild();
             ChildNode_InSym != nullptr;
             ChildNode_InSym = ChildNode_InSym->NextSibling()) {
          XMLElement *child_insym = ChildNode_InSym->ToElement();
          Symbol tmp_sym;
          const XMLAttribute *ChildSymAttr = child_insym->FirstAttribute();
          assert(strcmp(ChildSymAttr->Name(), "id") == 0);
          int id = stoi(child_insym->Attribute(ChildSymAttr->Name()));
          int num = stoi(child_insym->GetText());
          // tmp_sym.sym = sym;
          // tmp_sym.id = id;
          // tmp_sym.num = num;
          // cout << tmp_sym.sym << " " << tmp_sym.id << " " << tmp_sym.num
          //<< endl;
          string create_sym_resp = create_sym(C, sym, id, num);
          // res += "\n" + create_sym_resp;
          res += create_sym_resp;
        }
      }
    }
  }
  //<transactions> case
  else if (TopElmtName == "transactions") {
    const XMLAttribute *ChildAttrIni = TopElmt->FirstAttribute();
    assert(strcmp(ChildAttrIni->Name(), "id") == 0);
    int account_id = stoi(TopElmt->Attribute(ChildAttrIni->Name()));
    // cout << trans_id << endl;
    for (XMLNode *ChildNode = TopNode->FirstChild(); ChildNode != nullptr;
         ChildNode = ChildNode->NextSibling()) {
      XMLElement *child = ChildNode->ToElement();
      string childname = child->Name();
      // <order> case
      if (childname == "order") {
        // Order tmp_order;
        const XMLAttribute *ChildAttr = child->FirstAttribute();
        assert(strcmp(ChildAttr->Name(), "sym") == 0);
        string sym = child->Attribute(ChildAttr->Name());
        const XMLAttribute *ChildAttr2 = ChildAttr->Next();
        assert(strcmp(ChildAttr2->Name(), "amount") == 0);
        int amount = stoi(child->Attribute(ChildAttr2->Name()));
        const XMLAttribute *ChildAttr3 = ChildAttr2->Next();
        assert(strcmp(ChildAttr3->Name(), "limit") == 0);
        double limit = stod(child->Attribute(ChildAttr3->Name()));
        // tmp_order.sym = sym;
        // tmp_order.amount = amount;
        // tmp_order.limit = limit;
        // cout << tmp_order.sym << " " << tmp_order.amount << " "
        // << tmp_order.limit << endl;
        string order_resp = insert_order(C, account_id, sym, amount, limit);
        res += order_resp;
      }
      // <query> case
      else if (childname == "query") {
        // Query tmp_query;
        const XMLAttribute *ChildAttr = child->FirstAttribute();
        assert(strcmp(ChildAttr->Name(), "id") == 0);
        int id = stoi(child->Attribute(ChildAttr->Name()));
        // tmp_query.id = id;
        // cout << tmp_query.id << endl;
        string sql = "SELECT * FROM TRANSACTION WHERE (TRANS_ID = \'" +
                     to_string(id) + "\');";
        work query(*C);
        result read(query.exec(sql));
        query.commit();
        if (read.size() == 0) {
          string msg = "<error id=\"" + to_string(id) +
                       "\">Transaction ID doesnt exist</error>";
          res += msg;
        } else {
          vector<string> tmp = query_trans(C, id);
          for (int i = 0; i < int(tmp.size()); i++) {
            res += tmp[i];
          }
        }
      }
      // <cancel> case
      else if (childname == "cancel") {
        // Cancel tmp_cancel;
        const XMLAttribute *ChildAttr = child->FirstAttribute();
        assert(strcmp(ChildAttr->Name(), "id") == 0);
        int id = stoi(child->Attribute(ChildAttr->Name()));
        // tmp_cancel.id = id;
        // cout << tmp_cancel.id << endl;
        string sql = "SELECT * FROM TRANSACTION WHERE (TRANS_ID = \'" +
                     to_string(id) + "\');";
        work query(*C);
        result read(query.exec(sql));
        query.commit();
        if (read.size() == 0) {
          string msg = "<error id=\"" + to_string(id) +
                       "\">Transaction ID doesnt exist</error>";
          res += msg;
        } else {
          vector<string> tmp = cancel_trans(C, id);
          for (int i = 0; i < int(tmp.size()); i++) {
            res += tmp[i];
          }
        }
      }
    }
  }
  return res;
}
