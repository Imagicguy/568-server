#include "parse.h"

void Parse::init_connect(std::string request, std::string line) {
  parse_hostname(line);
  parse_type(line);
  parse_port(line);
}
void Parse::parse_type(std::string line) {
  type = line.substr(0, line.find(" "));
}

void Parse::parse_hostname(std::string line) {
  hostname = line.substr(line.find(" ") + 1, line.find(":"));
}

void Parse::parse_port(std::string line) {
  port = line.substr(line.find(" ") + 1, line.find("HTTP") - 1);
}
