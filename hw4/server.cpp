#include "socket.h"
#include "xml_parse.h"
#include <arpa/inet.h>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <pqxx/pqxx>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace pqxx;

void Processes(int socket_fd) {
  connection *C;
  try {
    C = new connection("dbname=stockdb user=postgres password=passw0rd");
    if (C->is_open()) {
      std::cout << "Opened database successfully: " << C->dbname() << std::endl;
    } else {
      std::cout << "Can't open database" << std::endl;
      exit(1);
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  char *recv_char;
  recv(socket_fd, &recv_char, sizeof(recv_char), 0); // NEED examing
  std::string resp_xml_str = resp_str(C, recv_char);
  std::cout << "response string: " << resp_xml_str << std::endl;
  send(socket_fd, resp_xml_str.c_str(), resp_xml_str.size(), 0);
  close(socket_fd);
  C->disconnect();
}

int main() {
  const char *port = "12345";
  const char *hostname = NULL;
  Socket server(hostname, port);
  int socket_fd = server.init_server();
  while (1) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int main_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (main_fd == -1) {
      std::cout << "error: main accept" << std::endl;
      continue;
    }
    // std::string ip = inet_ntoa(((struct sockaddr_in
    // *)&socket_addr)->sin_addr);

    std::thread(Processes, socket_fd).detach();
  }
}
