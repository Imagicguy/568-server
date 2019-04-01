CC=g++
CFLAGS=-ggdb3 -std=gnu++11 -Wall -Werror -pedantic -pthread
EXTRAFLAGS=-lpqxx -lpq

all: test test_client

test: server.cpp socket.h database.h database.cpp create.h create.cpp transaction.h xml_parse.h xml_parse.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o test server.cpp database.cpp create.cpp xml_parse.cpp tinyxml2.cpp $(EXTRAFLAGS)

test_client: client.cpp
	$(CC) $(CFLAGS) -o test_client client.cpp

clean:
	rm -f *.cpp~ *.h~ test test_client
