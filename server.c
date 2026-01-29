#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

//define a max
#define MAX_FDS 1000

//create an array to hold sockets
struct pollfd poll_fds[MAX_FDS];
int nfds = 0;

int main() {
    
    int server_fd;

    //create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //check if socket is successfull 
    if(server_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    //print socket on success
    printf("Socket created successfully! fd: %d\n", server_fd);
    
    //store the server's address information. Fields set below
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET; //address family
    addr.sin_port = htons(6380); //port 6379 in byte network order (htons -> converts int host to network 16-bit)
    addr.sin_addr.s_addr = htonl(INADDR_ANY); //any intterface in network order (htonl -> same as htons but long 32-bit)

    //reusable socket
    int val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        perror("Setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //now that we have the server's address we can bind to the socket using bind()
    //bind socket to address/port
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //start listening
    if(listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //print success
    printf("Server is listening on port 6380...\n");

    //struct for client address
    struct sockaddr_in client_addr = {0};
    socklen_t client_len = sizeof(client_addr); //length of client address

    //initiate variables
    poll_fds[0].fd = server_fd; //fd -> file descriptor, integer id for file, socket or devices beeing monitored
    poll_fds[0].events = POLLIN; //bitmask to define events the application wants to monitor
    nfds = 1;

    //infinet loop to keep accepting clients
    while(1) {

        //wait forever until something happens
        int timeout = -1;
        int ready = poll(poll_fds, nfds, timeout);

        if (ready < 0) {
            perror("poll failed");
            break;
        }

        //loop through all FDs to see which one is ready
        for(int i = 0; i < nfds; i++) {
            if(poll_fds[i].revents & POLLIN) {
                if(poll_fds[i].fd == server_fd) {
                    //handle new connections, accept 
                    //init client socket
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                    //try and accept client
                    if(client_fd >= 0) {
                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        poll_fds[nfds].fd = client_fd;
                        poll_fds[nfds].events = POLLIN;
                        nfds++;
                        printf("New client connected on fd %d\n", client_fd);
                    }
                } else {
                    //handle existing clients, read/write
                    char buffer[1024]; //define a buffer with limit of 1024 bytes
                    ssize_t n = read(poll_fds[i].fd, buffer, sizeof(buffer) - 1); //read into buffer
                    if(n <= 0) {
                        //client disconnected or error
                        printf("Client on fd %d disconnected\n", poll_fds[i].fd);
                        close(poll_fds[i].fd);
                        poll_fds[i] = poll_fds[nfds -1];
                        nfds--;
                        i--;
                    } else {
                        buffer[n] = '\0';
                        if(strncmp(buffer, "PING", 4) == 0) {
                            write(poll_fds[i].fd, "+PONG\r\n", 7);
                        }
                    }

                }
            }
        }
    }

    close(server_fd); //close socket
    return 0;
}