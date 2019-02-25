#include "parse.h"
void init_connect(std::string request, std::string line);
void parse_type(std::string line);
void parse_hostname(std::string line);
void parsef_port(std::string line);

void Parse::init_connect(std::string request, std::string line) {
  std::string res = request;
  parse_hostname(line);
  parse_type(line);
  parse_port(line);
}
void Parse::parse_type(std::string line) {
  type = line.substr(0, line.find(" "));
}

void Parse::parse_hostname(std::string line) {
  hostname =
      line.substr(line.find(" ") + 1, line.find(":") - line.find(" ") - 1);
}

void Parse::parse_port(std::string line) {
  port = line.substr(line.find(" ") + 1, line.find("HTTP") - 1);
  std::string temp = line.substr(line.find(":") + 1);
  port = temp.substr(0, temp.find(" "));
}
