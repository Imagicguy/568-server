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

int main(int argc, char *argv[]) {
  if (argc < 3)
    std::cout << "invalid input" << std::endl;
  const char *port = "12345";
  const char *hostname = argv[1];
  Socket client(hostname, port);
  int socket_fd = client.init_client();
  void *buffer;
  int len = 0; // = XML.length
  client.send_all(socket_fd, buffer, len);
}
