#include "socket.h"
#include "tinyxml2.h"
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
using namespace std;
using namespace tinyxml2;

int main(int argc, char *argv[]) {
  if (argc < 2)
    cout << "invalid input" << endl;
  const char *port = "12345";
  const char *hostname = argv[1];
  Socket client(hostname, port);
  int socket_fd = client.init_client();
  const char *buffer =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?><create><account id=\"100\" "
      "balance=\"10000.5\"/><account id=\"101\" balance=\"5000.3\"/><account "
      "id=\"102\" balance=\"1000.6\"/><symbol sym=\"SPY\"><account "
      "id=\"101\">200</account><account "
      "id=\"102\">300</account></symbol><symbol sym=\"GOLD\"><account "
      "id=\"100\">100</account><account "
      "id=\"102\">200</account></symbol></create>";
  send(socket_fd, buffer, 50000, 0);

  // cout << buffer << endl;

  char recv_buf[50000];
  recv(socket_fd, &recv_buf, 50000, 0);
  // cout << recv_buf << endl;

  XMLDocument xml;
  string pSource = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><results>" +
                   string(recv_buf) + "</results>";
  // cout << pSource << endl;
  xml.Parse(pSource.c_str());
  // cout << "xml.Print():" << endl;
  xml.Print();

  close(socket_fd);
  return 0;
}
