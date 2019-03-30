#include "socket.h"
#include <arpa/inet.h>
#include <cstring>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2)
    std::cout << "invalid input" << std::endl;
  const char *port = "12345";
  const char *hostname = argv[1];
  Socket client(hostname, port);
  int socket_fd = client.init_client();
  const char *buffer =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?><create><account id=\"123\" "
      "balance=\"1000\"/><account id=\"456\" balance=\"2000\"/><account "
      "id=\"123\" balance=\"3000\"/><symbol sym=\"SPY\"><account "
      "id=\"123\">100000</account></symbol><symbol sym=\"BTC\"><account "
      "id=\"456\">90000</account><account "
      "id=\"789\">70000</account></symbol><symbol sym=\"T5asdf\"><account "
      "id=\"789\">80000</account></symbol><symbol sym=\"SPY\"><account "
      "id=\"123\">70000</account><account "
      "id=\"789\">60000</account></symbol></create>";
  int len = strlen(buffer); // = XML.length
  client.send_all(socket_fd, buffer, len);

  char *recv_buf;
  recv(socket_fd, &recv_buf, sizeof(recv_buf), 0);
  std::cout << recv_buf << std::endl;
  close(socket_fd);
  return 0;
}
