TARGETS=main proxy

main:
	g++ -g -Wall -pthread -o main main.cpp proxy.cpp proxy.h response.h socket.h parse.cpp parse.h cache.h cache.cpp response.h
clean:
	rm -f $(TARGETS)
