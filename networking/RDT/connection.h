#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <cstring> // memset, etc.
#include <signal.h>
#include <netdb.h> // hostent, etc.
#include <arpa/inet.h> // inet_htop
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <iomanip>
#include <fcntl.h>

#define SERVER_IP "127.0.0.1"
#define SYN 1
#define SYNACK 2
#define FIN 3
#define FINACK 4
#define LASTACK 5
#define DATA_REQ 6
#define DATA_SENDING 7


#define SYN 1
#define SYNACK 2
#define FIN 3
#define FINACK 4
#define LASTACK 5
#define DATA_REQ 6
#define DATA_SENDING 7
#define ACK 8
#define BADFILE 9

//#define MAX_TIMEOUT 100
#define MAX_TIMOUT_USEC 50000
#define DATA_SIZE 980

using namespace std;

struct header {
    int length;

};
typedef struct packet {
    struct header my_head;
    char data[DATA_SIZE];
        int seq;
    int type;
    int ack;
    timeval sendTime;
    int numPacketsInFile;

}packet;
