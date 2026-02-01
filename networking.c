#include "server.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

//here we find the networking logic that handles the connection/disconnection between client and server

//initiate server
int server_init(RedisServer *s, int port) {
    s -> fd = socket(AF_INET, SOCK_STREAM, 0);
    if(s -> fd < 0) return -1;
    
    int val = 1;
    setsockopt(s -> fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(s -> fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) return -1;
    if(listen(s -> fd, 10) < 0) return -1;

    s -> fds[0].fd = s -> fd;
    s -> fds[0].events = POLLIN;
    s -> nfds = 1;
    return 0;
}


//add client to server
void server_add_client(RedisServer *s, int client_fd) {
    if(s -> nfds < MAX_FDS) {
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        s -> fds[s -> nfds].fd = client_fd;
        s -> fds[s -> nfds].events = POLLIN;
        s -> nfds++;
    }
}