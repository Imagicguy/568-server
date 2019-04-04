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
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions "
      "id=\"456\"><order sym=\"SPY\" amount=\"-1\" limit=\"1\"/><order "
      "sym=\"SPY\" amount=\"-2\" limit=\"2\"/><order sym=\"SPY\" amount=\"-3\" "
      "limit=\"3\"/><order sym=\"SPY\" amount=\"-4\" limit=\"4\"/><order "
      "sym=\"SPY\" amount=\"-5\" limit=\"5\"/><order sym=\"SPY\" amount=\"-6\" "
      "limit=\"6\"/><order sym=\"SPY\" amount=\"-7\" limit=\"7\"/><order "
      "sym=\"SPY\" amount=\"-8\" limit=\"8\"/><order sym=\"SPY\" amount=\"-9\" "
      "limit=\"9\"/><order sym=\"SPY\" amount=\"-10\" limit=\"10\"/><order "
      "sym=\"BTC\" amount=\"10\" limit=\"30\"/><order sym=\"BTC\" amount=\"9\" "
      "limit=\"27\"/><order sym=\"BTC\" amount=\"8\" limit=\"24\"/><order "
      "sym=\"BTC\" amount=\"7\" limit=\"21\"/><order sym=\"BTC\" amount=\"6\" "
      "limit=\"18\"/><order sym=\"BTC\" amount=\"5\" limit=\"15\"/><order "
      "sym=\"BTC\" amount=\"4\" limit=\"12\"/><order sym=\"BTC\" amount=\"3\" "
      "limit=\"9\"/><order sym=\"BTC\" amount=\"2\" limit=\"6\"/><order "
      "sym=\"BTC\" amount=\"1\" limit=\"3\"/></transactions>";

  send(socket_fd, buffer, 50000, 0);

  // cout << buffer << endl;

  char recv_buf[50000];
  recv(socket_fd, &recv_buf, 50000, 0);

  XMLDocument xml;
  string pSource = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><results>" +
                   string(recv_buf) + "</results>";
  //  cout << pSource << endl;
  xml.Parse(pSource.c_str());
  // cout << "xml.Print():" << endl;
  xml.Print();

  close(socket_fd);
  return 0;
}
