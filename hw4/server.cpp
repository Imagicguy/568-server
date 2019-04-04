#include "ThreadPool.h"
#include "database.h"
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

void Processes(int main_fd) {
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
  char recv_char[50000];
  recv(main_fd, recv_char, 50000, 0); // NEED examing
  cout << recv_char << endl;
  std::string resp_xml_str = resp_str(C, recv_char);
  std::cout << "response string: " << resp_xml_str << std::endl;
  send(main_fd, resp_xml_str.c_str(), resp_xml_str.size(), 0);
  close(main_fd);
  C->disconnect();
}

int main() {
  const char *port = "12345";
  const char *hostname = NULL;
  Socket server(hostname, port);
  int socket_fd = server.init_server();
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

  cleanTable(C);
  initTable(C);
  C->disconnect();
  ThreadPool pool(100);
  while (1) {
    std::cout << "Waiting for connection on port " << port << std::endl;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int main_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (main_fd == -1) {
      std::cout << "error: main accept" << std::endl;
      continue;
    }
    pool.enqueue(Processes, main_fd);
    // std::thread(Processes, main_fd).detach();
  }
}
