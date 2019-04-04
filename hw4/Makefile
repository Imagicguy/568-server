CC=g++
CFLAGS=-ggdb3 -std=gnu++11 -Wall -Werror -pedantic -pthread
EXTRAFLAGS=-lpqxx -lpq

all: server

server: server.cpp socket.h database.h database.cpp create.h create.cpp transaction.h transaction.cpp xml_parse.h xml_parse.cpp tinyxml2.h tinyxml2.cpp ThreadPool.h
	$(CC) $(CFLAGS) -o server server.cpp database.cpp create.cpp transaction.cpp xml_parse.cpp tinyxml2.cpp $(EXTRAFLAGS)

clean:
	rm -f *.cpp~ *.h~ server
