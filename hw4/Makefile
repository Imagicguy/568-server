CC=g++
CFLAGS=-ggdb3 -std=gnu++11 -Wall -Werror -pedantic -pthread
EXTRAFLAGS=-lpqxx -lpq

all: server client1 client2 client3 client4 client5 client6 client7 client8

server: server.cpp socket.h database.h database.cpp create.h create.cpp transaction.h transaction.cpp xml_parse.h xml_parse.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o server server.cpp database.cpp create.cpp transaction.cpp xml_parse.cpp tinyxml2.cpp $(EXTRAFLAGS)

client1: client1.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client1 client1.cpp tinyxml2.cpp

client2: client2.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client2 client2.cpp tinyxml2.cpp

client3: client3.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client3 client3.cpp tinyxml2.cpp

client4: client4.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client4 client4.cpp tinyxml2.cpp

client5: client5.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client5 client5.cpp tinyxml2.cpp

client6: client6.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client6 client6.cpp tinyxml2.cpp

client7: client7.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client7 client7.cpp tinyxml2.cpp

client8: client8.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o client8 client8.cpp tinyxml2.cpp

clean:
	rm -f *.cpp~ *.h~ server client1 client2 client3 client4 client5 client6 client7 client8
