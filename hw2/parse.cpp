#include "parse.h"
void init_connect(std::string request, std::string line);
void init_get(std::string request, std::string line);
void parse_type(std::string line);
void parse_hostname(std::string line);
void parsef_port(std::string line);

void Parse::init_connect(std::string request, std::string line) {
  header = line;
  std::string res = request;
  parse_hostname_c(line);
  parse_type(line);
  port = "443";
}

void Parse::init_get(std::string requests, std::string line) {
  header = line;
  request = requests;
  parse_hostname_g(requests);
  parse_type(line);
  port = "80";
}
void Parse::parse_type(std::string line) {
  type = line.substr(0, line.find(" "));
}
void Parse::parse_hostname_g(std::string request) {
  std::string temp = request.substr(request.find("Host: ") + 6);
  std::cout << "temp is " << temp << std::endl;
  hostname = temp.substr(0, temp.find("\r\n"));
}
void Parse::parse_hostname_c(std::string line) {
  hostname =
      line.substr(line.find(" ") + 1, line.find(":") - line.find(" ") - 1);
}

void Parse::parse_port_c(std::string line) {
  port = line.substr(line.find(" ") + 1, line.find("HTTP") - 1);
  std::string temp = line.substr(line.find(":") + 1);
  port = temp.substr(0, temp.find(" "));
}
