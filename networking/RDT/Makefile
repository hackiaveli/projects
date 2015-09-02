CC = g++
CFLAGS = -g -Wall -Wextra -Werror

all: sender client

SENDER_SOURCES = \
	host.cpp \
	
SENDER_OBJECTS = $(subst .cpp,.o,$(SENDER_SOURCES))

sender: $(SENDER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SENDER_OBJECTS)

CLIENT_SOURCES = \
	client.cpp \
	
CLIENT_OBJECTS = $(subst .cpp,.o,$(CLIENT_SOURCES))

client: $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJECTS)


clean:
	rm -fr *.o *~ *.bak *.tar.gz core *.core *.tmp sender client 