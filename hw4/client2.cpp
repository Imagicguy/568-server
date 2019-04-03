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
      "id=\"123\"><order sym=\"BTC\" amount=\"300\" "
      "limit=\"125\"/></transactions>";

  // int len = strlen(buffer); // = XML.length
  // client.send_all(socket_fd, buffer, len);
  int status = send(socket_fd, buffer, 50000, 0);
  cout << "send_status" << status << endl;

  cout << buffer << endl;

  char recv_buf[50000];
  status = recv(socket_fd, &recv_buf, 50000, 0);
  // cout << recv_buf << endl;

  XMLDocument xml;
  string pSource = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><results>" +
                   string(recv_buf) + "</results>";
  cout << pSource << endl;
  xml.Parse(pSource.c_str());
  cout << "xml.Print():" << endl;
  xml.Print();

  close(socket_fd);
  return 0;
}
