#include "main.h"
#include "proxy.h"
#include "socket.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
// socket proxy  parse request

void Processes(Proxy arg, int ID);

int main() {
  int id = 0;
  const char *port = "8080";
  const char *hostname = NULL;
  Socket server(hostname, port);
  int socket_fd = server.init_server();
  while (1) {
    id++;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int main_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (main_fd == -1) {
      std::cout << "error: main accept" << std::endl;
      continue;
    }
    std::string ip;
    ip = inet_ntoa(((struct sockaddr_in *)&socket_addr)->sin_addr);

    Proxy proxy(socket_fd, main_fd, ip, port);
    std::thread(Processes, proxy, id).detach();
  }
}
void Processes(Proxy arg, int ID) { arg.process(ID); }
