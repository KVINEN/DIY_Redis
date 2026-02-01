#include "server.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


//main. Here we find the infinit loop to keep the server open 
int main() {
    RedisServer server;
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    if(server_init(&server, 6380) < 0) {
        perror("Initialization failed");
        return 1;
    }

    printf("Server listening on port 6380...\n");

    //infinet loop to keep accepting clients
    while(1) {

        int ready = poll(server.fds, server.nfds, -1);
        
        if (ready < 0) {
            perror("poll failed");
            break;
        }

        //loop through all FDs to see which one is ready
        for(int i = 0; i < server.nfds; i++) {
            if(server.fds[i].revents & POLLIN) {
                if(server.fds[i].fd == server.fd) {
                    //handle new connections, accept 
                    //init client socket
                    int client_fd = accept(server.fd, (struct sockaddr *)&client_addr, &client_len);
                    //try and accept client
                    if(client_fd >= 0) {
                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        server.fds[server.nfds].fd = client_fd;
                        server.fds[server.nfds].events = POLLIN;
                        server.nfds++;
                        printf("New client connected on fd %d\n", client_fd);
                    }
                } else {
                    //handle existing clients, read/write
                    char buffer[1024]; //define a buffer with limit of 1024 bytes
                    ssize_t n = read(server.fds[i].fd, buffer, sizeof(buffer) - 1); //read into buffer
                    if(n <= 0) {
                        //client disconnected or error
                        printf("Client on fd %d disconnected\n", server.fds[i].fd);
                        close(server.fds[i].fd);
                        server.fds[i] = server.fds[server.nfds -1];
                        server.nfds--;
                        i--;
                    } else {
                        buffer[n] = '\0';
                        if(strncmp(buffer, "PING", 4) == 0) {
                            write(server.fds[i].fd, "+PONG\r\n", 7);
                        }
                    }

                }
            }
        }
    }

    close(server.fd); //close socket
    return 0;
}