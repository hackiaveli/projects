CC=g++
CFLAGS= -g 

all: server

server: server.o
	$(CC) server.o -o server

server.o: server.cpp
	$(CC) $(CFLAGS) -c server.cpp

clean:
	rm -r server.o 
	rm -r server
