#ifndef SERVER_H
#define SERVER_H

#include <poll.h>
#include <netinet/in.h>

#define MAX_FDS 1000

//define struct for other files
typedef struct {
    int fd;
    struct pollfd fds[MAX_FDS];
    int nfds;
    struct sockaddr_in addr;
} RedisServer;

//func prototypes: tells the compiler these exist
int server_init(RedisServer *server, int port);
void server_run(RedisServer *server);
void handle_client_activity(RedisServer *server, int index);

#endif