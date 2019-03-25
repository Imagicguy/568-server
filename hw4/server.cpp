#include "XML.h"
#include "socket.h"
#include <arpa/inet.h>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

void Processes(XML arg, int ID) { arg.process(ID); }

int main() {
  int id = 0;
  const char *port = "12345";
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

    XML request();
    std::thread(Processes, request, id).detach();
  }
}
