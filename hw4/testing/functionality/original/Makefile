CC=g++
CFLAGS=-ggdb3 -std=gnu++11 -Wall -Werror -pedantic -pthread
EXTRAFLAGS=-lpqxx -lpq

all: client1 client2 client3 client4 client5

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

clean:
	rm -f *.cpp~ *.h~ client1 client2 client3 client4 client5
